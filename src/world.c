#include "world.h"
#include "defines.h"
#include "chunk.h"
#include "noise.h"
#include "util.h"
#include <pthread.h>
#include <unistd.h> // For sysconf

extern State state;

typedef struct
{
    int thread_id;
    int chunk_start;
    int chunk_end;
    fnl_state noise;
} ThreadData;

int num_chunks_loaded = 0;

void update_process()
{
    num_chunks_loaded++;
    print_i(num_chunks_loaded);
}

void *generate_chunks(void *arg)
{
    ThreadData *data = (ThreadData *)arg;

    for (int i = data->chunk_start; i < data->chunk_end; i++)
    {
        int offset_x = (i % (int)sqrt(CHUNKS_COUNT)) * CHUNK_SIZE;
        int offset_z = (i / (int)sqrt(CHUNKS_COUNT)) * CHUNK_SIZE;

        Chunk *chunk = chunk_create(data->noise, offset_x, offset_z);
        state.chunks[i] = *chunk;

        update_process();
    }

    return NULL;
}

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

    int num_processors = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_processors < 1)
    {
        num_processors = 1;
    }

    pthread_t threads[num_processors];
    ThreadData thread_data[num_processors];

    int chunks_per_thread = CHUNKS_COUNT / num_processors;
    int remainder_chunks = CHUNKS_COUNT % num_processors;

    for (int i = 0; i < num_processors; i++)
    {
        thread_data[i].thread_id = i;
        thread_data[i].chunk_start = i * chunks_per_thread;
        thread_data[i].chunk_end = (i + 1) * chunks_per_thread;

        if (i == num_processors - 1)
        {
            thread_data[i].chunk_end += remainder_chunks;
        }

        thread_data[i].noise = noise;

        pthread_create(&threads[i], NULL, generate_chunks, &thread_data[i]);
    }

    for (int i = 0; i < num_processors; i++)
    {
        pthread_join(threads[i], NULL);
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