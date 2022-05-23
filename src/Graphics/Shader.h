/* date = December 8th 2021 11:25 am */
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>

std::string get_file_contents(const char* filename);

class Shader{
    public:
    GLuint ID;
    Shader(const char* vertexFile, const char* fragmentFile);

    void Activate();
    void Deactivate();
    void Delete();
    private:
    void compileErrors(unsigned int shader, const char* type);

    public:
    void uploadVec4(const char* varName, const glm::vec4& vec4) const;
    void uploadVec3(const char* varName, const glm::vec3& vec3) const;
    void uploadVec2(const char* varName, const glm::vec2& vec2) const;
    void uploadIVec4(const char* varName, const glm::ivec4& vec4) const;
    void uploadIVec3(const char* varName, const glm::ivec3& vec3) const;
    void uploadIVec2(const char* varName, const glm::ivec2& vec2) const;
    void uploadFloat(const char* varName, float value) const;
    void uploadInt(const char* varName, int value) const;
    void uploadIntArray(const char* varName, int length, const int* array) const;
    void uploadUInt(const char* varName, unsigned int value) const;
    void uploadBool(const char* varName, bool value) const;


    void uploadMat4(const char* varName, const glm::mat4& mat4) const;
    void uploadMat3(const char* varName, const glm::mat3& mat3) const;
};


#endif



