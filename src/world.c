#include "world.h"
#include "defines.h"
#include "chunk.h"
#include "noise.h"

extern State state;

void world_generate()
{
    state.chunks = malloc(CHUNKS_COUNT * sizeof(Chunk));

    fnl_state noise = fnlCreateState();
    noise.seed = 1337;
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    noise.frequency = 0.005f;
    noise.octaves = 5;
    noise.lacunarity = 2.0f;
    noise.gain = 0.5f;
    noise.fractal_type = FNL_FRACTAL_FBM;

    for (int i = 0; i < CHUNKS_COUNT; i++)
    {
        int offset_x = (i % (int)sqrt(CHUNKS_COUNT)) * CHUNK_SIZE;
        int offset_z = (i / (int)sqrt(CHUNKS_COUNT)) * CHUNK_SIZE;

        Chunk *chunk = chunk_create(noise, offset_x, offset_z);
        state.chunks[i] = *chunk;
    }
}

void world_update()
{
    for (int i = 0; i < CHUNKS_COUNT; i++)
    {
        chunk_update(&state.chunks[i]);
    }
}

void world_render()
{
    for (int i = 0; i < CHUNKS_COUNT; i++)
    {
        if (state.chunks[i].visible)
        {
            chunk_render(&state.chunks[i]);
        }
    }
}
