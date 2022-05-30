#ifndef MINECRAFT_VERTEX_H
#define MINECRAFT_VERTEX_H
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Vertex{
    uint8_t xz;
    uint8_t y;
    uint8_t texPos;
    uint8_t data; // 3 bits face, 4 bits light, 1 bit transparent
};

#endif //MINECRAFT_VERTEX_H
