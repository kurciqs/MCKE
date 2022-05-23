#include "Camera.h"

Camera::Camera(unsigned int width, unsigned int height, glm::vec3 position) {
    m_width = width;
    m_height = height;
    Position = position;
}

void Camera::Update() {
    /*Updates view and projection*/
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    view = glm::lookAt(Position, Position + Orientation, Up);

    projection = glm::perspective(glm::radians(fov), ((float)m_width/(float)m_height), near, far);
    cameraMatrix = projection * view;
}

void Camera::Upload(Shader &shader, const char *uniform_name) {
    shader.uploadMat4(uniform_name, cameraMatrix);
}

void Camera::Input() {
    Window* window = Input::getWindow();

    if (!Input::isKeyDown(GLFW_KEY_RIGHT_ALT)) {
        Input::cursorView(true);

        if (firstClick) {
            Input::setCursorPos((float) window->width / 2.0f, (float) window->height / 2.0f);
            firstClick = false;
        }

        double mouseX;
        double mouseY;
        Input::getCursorPos(mouseX, mouseY);

        float rotx = sensitivity * (float)(mouseY - (window->height / 2.0f)) / window->height;
        float roty = sensitivity * (float)(mouseX - (window->width / 2.0f)) / window->width;

        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotx), glm::normalize(glm::cross(Orientation, Up)));

        if ((glm::angle(newOrientation, Up) >= glm::radians(7.5f) && glm::angle(newOrientation, -Up) >= glm::radians(7.5f)) ) {
            Orientation = newOrientation;
        }

        Orientation = glm::rotate(Orientation, glm::radians(-roty), Up);
        Input::setCursorPos((float) window->width / 2.0f, (float) window->height / 2.0f);
    }
    else{
        Input::cursorView(false);
        firstClick = true;
    }
}

void Camera::setWindowParams(Window* window) {
    m_width = window->width;
    m_height = window->height;
}

glm::vec3 Camera::getPos() {
    return Position;
}

void Camera::setPos(glm::vec3 pos) {
    Position = pos;
}
