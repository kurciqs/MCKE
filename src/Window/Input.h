#ifndef _INPUT_H
#define _INPUT_H
#include <iostream>
#include "Window/Window.h"

namespace Callback{
    void errorCallBack(int error, const char* description);
    void resizeCallback(GLFWwindow* nativeWindow, int newWidth, int newHeight);
}

namespace Input{
    static Window* mWindow;
    Window* getWindow();

    void BindWindow(Window* window);
    void checkInputs();

    bool isMouseButtonDown(int mouseButton);
    bool isKeyDown(int key);

    void cursorView(bool hidden);
    void setCursorPos(float x, float y);
    void getCursorPos(double &x, double &y);
    bool isMouseButtonUp(int mouseButton);

    float InputX();
    float InputY();
}

#endif //_INPUT_H
