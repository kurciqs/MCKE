#version 460 core

layout (location = 0) in vec3 a_position;

uniform mat4 model;
uniform mat4 cam;

void main() {
    gl_Position = (vec4(a_position, 1.0));
}