#include "App.h"

//path to executable, needed for working directory
std::filesystem::path getexepath()
{
#ifdef _WIN32
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    return path;
#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::string(result, (count > 0) ? count : 0);
#endif
}

void GLAPIENTRY MessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
{
    fprintf( stderr, "[OPENGL_ERROR]: %s type = 0x%x, severity = 0x%x, message = %s\n",
             ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
             type, severity, message );
}

namespace App{
    int Load() {
        //Set the working directory correctly --- for me it would be: F:\\Programming\\cpp\\MinecraftClone\\cmake-build-debug
        //WARNING: Make sure you keep the exe in cmake-build-debug!!!
        std::filesystem::current_path(getexepath().parent_path());
        if (!glfwInit()){
            std::cout << "[ERROR]: Failed to load GLFW!" << std::endl;
            return -1;
        }
        window = Window::createWindow(1920, 1280, "Minecraft | FPS: 0");
        if (window == nullptr)
        {
            return -1;
        }
        gladLoadGL();
        glViewport(0, 0, window->width, window->height);

        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        Input::BindWindow(window);

        glfwSwapInterval(1);
        BlockParser textureParser("../res/textures/format/TexUVs.yaml", "../res/textures/format/blocks.yaml");

        world = new World(window, 42069, textureParser, "../world");
        return 0;
    }

    void Play() {
        static int renderDistance = 12;
        world->Load();
        world->GenerateChunks(renderDistance);
        double prevTime = (float)glfwGetTime() - 0.16f;
        double curTime;
        double timeDiff;
        uint32_t counter = 0;
        world->Update(0.02f);

        while(!glfwWindowShouldClose(window->nativeWindow)){
            // game loop only gets called if window is focused, only poll events are accepted!
            if (window->isFocused()) {
                curTime = glfwGetTime();
                timeDiff = curTime - prevTime;
                counter++;

                glm::vec4 sky = world->skyColor * world->time;
                glClearColor(sky.r, sky.g, sky.b, sky.a);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                Input::checkInputs();
                world->Update(0.02f);
                world->RegenerateChunks(renderDistance); /// NOTE: CALL UPDATE() @BEFORE REGENERATE()!!!

                world->Render();
                glfwSwapBuffers(window->nativeWindow);

                if (timeDiff >= 1.0f/30.0f){
                    std::string FPS = std::to_string((int)std::round((1.0 / timeDiff) * counter));
                    std::string winTitle = std::string("Minecraft | FPS: ") + FPS;
                    glfwSetWindowTitle(window->nativeWindow, winTitle.c_str());
                    prevTime = curTime;
                    counter = 0;
                }
            }

            glfwPollEvents();
        }
    }

    void Free() {
        world->Delete();
        Window::freeWindow(window);
        delete world;
        glfwTerminate();
    }
}