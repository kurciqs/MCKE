#ifndef MINECRAFT_CAMERA_H
#define MINECRAFT_CAMERA_H
#include "Window/Input.h"
#include "Window/Window.h"
#include "Graphics/Shader.h"

class Camera {
public:
    Camera(unsigned int width, unsigned int height, glm::vec3 position);
    void Update();
    void Upload(Shader& shader, const char* uniform_name);
    void Input();
    void setWindowParams(Window* window);
    float fov = 45.0f;
    float near = 0.05f;
    float far = 1000.0f;
    float sensitivity = 225.0f;
    glm::mat4 cameraMatrix = glm::mat4(1.0f);
    glm::vec3 getPos();
    void setPos(glm::vec3 pos);
    glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Position;
private:
    bool firstClick = true;
    unsigned int m_width, m_height;
};


#endif //MINECRAFT_CAMERA_H
