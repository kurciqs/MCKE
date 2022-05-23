#ifndef MINECRAFT_APP_H
#define MINECRAFT_APP_H
#include <filesystem>
#include "World/World.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <limits.h>
#include <unistd.h>     //readlink
#endif

namespace App {
    static Window* window;
    static World *world;
    int Load();
    void Play();
    void Free();
}


#endif //MINECRAFT_APP_H
