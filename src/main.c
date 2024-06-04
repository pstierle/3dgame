#include "gfx.h"
#include "defines.h"
#include "camera.h"
#include "shader.h"
#include "window.h"
#include "renderer.h"
#include "cglm/cglm.h"
#include "util.h"

State state;

int main()
{
    window_init();
    shader_init();
    renderer_init();
    camera_init();

    while (!glfwWindowShouldClose(state.window.handle))
    {
        // input

        window_input();

        // update

        camera_update();
        renderer_update();

        glfwSwapBuffers(state.window.handle);
    }

    glfwTerminate();
    return 0;
}