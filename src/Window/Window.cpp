#include "Window.h"
#include "Window/Input.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_img.h"

Window* Window::createWindow(unsigned int WIDTH, unsigned int HEIGHT, const char* name){

    auto* res = new Window();
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    res->maxWidth = mode->width;
    res->maxHeight = mode->height;


//    res->nativeWindow = glfwCreateWindow(WIDTH, HEIGHT, name, primaryMonitor, nullptr);
    res->nativeWindow = glfwCreateWindow(WIDTH, HEIGHT, name, nullptr, nullptr);
    if (res->nativeWindow == nullptr)
    {
        std::cout << "[ERROR]: Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    res->originalWidth = WIDTH;
    res->originalHeight = HEIGHT;
    res->width = WIDTH;
    res->height = HEIGHT;

    glfwMakeContextCurrent(res->nativeWindow);
    glfwSetWindowUserPointer(res->nativeWindow, (void*)res);

    GLFWimage images[1];
    images[0].pixels = stbi_load("../res/img/Icon.png", &images[0].width, &images[0].height, nullptr, 4); //rgba channels
    glfwSetWindowIcon(res->nativeWindow, 1, images);
    stbi_image_free(images[0].pixels);


    res->installMainCallbacks();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    return res;
}

void Window::freeWindow(Window* window){
    if (window){
        glfwDestroyWindow(window->nativeWindow);
        delete window;
    }
}

void Window::installMainCallbacks() const{
    if (nativeWindow != nullptr)
    {
        glfwSetWindowSizeCallback(nativeWindow, Callback::resizeCallback);
        glfwSetErrorCallback((GLFWerrorfun)(Callback::errorCallBack));
    }
}

void Window::close() const{
    if (nativeWindow != nullptr)
    {
        glfwSetWindowShouldClose(nativeWindow, GLFW_TRUE);
    }
}

void Window::enterFullScreen(){
    fullscreen = true;
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(nativeWindow, monitor, mode->width/2, mode->height/2, mode->width, mode->height, mode->refreshRate);
    glfwSwapInterval(0);
    glfwSwapInterval(1);
}

void Window::exitFullScreen(){
    fullscreen = false;
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(nativeWindow, nullptr,  100, 100, originalWidth, originalHeight, mode->refreshRate);
    glfwSwapInterval(0);
    glfwSwapInterval(1);
}

bool Window::isFocused() const {
    return glfwGetWindowAttrib(nativeWindow, GLFW_FOCUSED);
}
