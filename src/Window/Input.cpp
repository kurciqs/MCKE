#include "Input.h"
namespace Callback{
    void resizeCallback(GLFWwindow* nativeWindow, int newWidth, int newHeight)
    {
        auto* window = (Window*)glfwGetWindowUserPointer(nativeWindow);
        if (newWidth != 0 && newHeight != 0) {
            window->width = newWidth;
            window->height = newHeight;
            glViewport(0, 0, newWidth, newHeight);
        }
    }

    void errorCallBack(int error, const char* description){
        std::cout << "[GLFW_ERROR]: " << error << " |\n" << description << "\n";
    }
}
namespace Input {

    void BindWindow(Window *window) {
        mWindow = window;
    }

    bool isKeyDown(int key) {
        return (glfwGetKey(mWindow->nativeWindow, key) == GLFW_PRESS);
    }

    bool isMouseButtonDown(int mouseButton) {
        return (glfwGetMouseButton(mWindow->nativeWindow, mouseButton) == GLFW_PRESS);
    }

    bool isMouseButtonUp(int mouseButton) {
        return (glfwGetMouseButton(mWindow->nativeWindow, mouseButton) == GLFW_RELEASE);
    }

    void cursorView(bool hidden) {
        if (hidden)
            glfwSetInputMode(mWindow->nativeWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        else
            glfwSetInputMode(mWindow->nativeWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void setCursorPos(float x, float y) {
        glfwSetCursorPos(mWindow->nativeWindow, x, y);
    }

    void getCursorPos(double &x, double &y) {
        glfwGetCursorPos(mWindow->nativeWindow, &x, &y);
    }

    float InputX() {
        int result = 0;
        if (isKeyDown(GLFW_KEY_A))
            result += 1;
        if (isKeyDown(GLFW_KEY_D))
            result += -1;
        return result;
    }

    float InputY() {
        int result = 0;
        if (isKeyDown(GLFW_KEY_S))
            result += -1;
        if (isKeyDown(GLFW_KEY_W))
            result += 1;
        return result;
    }

    void checkInputs() {
        if (Input::isKeyDown(GLFW_KEY_ESCAPE)){
            mWindow->close();
        }
        if (Input::isKeyDown(GLFW_KEY_F11) && !mWindow->fullscreen) {
            mWindow->enterFullScreen();
        }
        else if (Input::isKeyDown(GLFW_KEY_F11) && mWindow->fullscreen) {
            mWindow->exitFullScreen();
        }
    }

    Window *getWindow() {
        return mWindow;
    }
}