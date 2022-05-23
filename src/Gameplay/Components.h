#ifndef MINECRAFT_COMPONENTS_H
#define MINECRAFT_COMPONENTS_H
#include "glm/glm.hpp"

struct RigidBody
{
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    bool onGround;
    bool isFlying;
};

struct Transform
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

#endif //MINECRAFT_COMPONENTS_H
