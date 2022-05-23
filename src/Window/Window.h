/* date = December 23rd 2021 3:22 pm */
#ifndef WINDOW_H
#define WINDOW_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb/stb_img.h"


struct Window {
    GLFWwindow* nativeWindow;
    unsigned int width;
    unsigned int height;
    unsigned int maxWidth;
    unsigned int maxHeight;
    unsigned int originalWidth = 1500;
    unsigned int originalHeight = 1200;
    [[nodiscard]] bool isFocused() const;
    bool fullscreen = false;
    void installMainCallbacks() const;
    void close() const;
    void enterFullScreen();
    void exitFullScreen();

    static Window* createWindow(unsigned int w, unsigned int h, const char* name);
    static void freeWindow(Window* window);
};

#endif //WINDOW_H


