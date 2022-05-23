#include "Shader.h"

// Reads a text file and outputs a string with everything in the text file
std::string get_file_contents(const char* filename)
{
    //*some cpp wizardry*
    std::ifstream in(filename, std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    else{
        std::cout << "[ERROR]: Shader file reading failed!" << std::endl;
        exit(1);
    }
}

// Constructor that build the Shader Program from 2 different shaders
Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
    // Read vertexFile and fragmentFile and store the strings
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);

    // Convert the shader source strings into character arrays
    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    // Create Vertex Shader Object and get its reference (OpenGL remembers it)
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Attach Vertex Shader source to the Vertex Shader Object (1 for string length)
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    // Compile the Vertex Shader into machine code
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    // Create Fragment Shader Object and get its reference
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // Attach Fragment Shader source to the Fragment Shader Object (1 for string length)
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    // Compile the Vertex Shader into machine code
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    // Create Shader Program Object and get its reference
    ID = glCreateProgram();
    // Attach the Vertex and Fragment Shaders to the Shader Program
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    // Wrap-up/Link all the shaders together into the Shader Program
    glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");

    // Delete the now useless Vertex and Fragment Shader objects  (they are now connected to the ShaderProgram)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


void Shader::Activate(){
    //just use it
    glUseProgram(ID);
}

void Shader::Deactivate(){
    //just don't use it
    glUseProgram(0);
}


void Shader::Delete(){
    //del
    glDeleteProgram(ID);
}

void Shader::uploadVec4(const char* varName, const glm::vec4& vec4) const
{
    glUniform4f(glGetUniformLocation(ID,  varName), vec4.x, vec4.y, vec4.z, vec4.w);
}

void Shader::uploadVec3(const char* varName, const glm::vec3& vec3) const
{
    glUniform3f(glGetUniformLocation(ID,  varName), vec3.x, vec3.y, vec3.z);
}

void Shader::uploadVec2(const char* varName, const glm::vec2& vec2) const
{
     glUniform2f(glGetUniformLocation(ID,  varName), vec2.x, vec2.y);
}

void Shader::uploadIVec4(const char* varName, const glm::ivec4& vec4) const
{
     glUniform4i(glGetUniformLocation(ID,  varName), vec4.x, vec4.y, vec4.z, vec4.w);
}

void Shader::uploadIVec3(const char* varName, const glm::ivec3& vec3) const
{
    glUniform3i(glGetUniformLocation(ID,  varName), vec3.x, vec3.y, vec3.z);
}

void Shader::uploadIVec2(const char* varName, const glm::ivec2& vec2) const
{
    glUniform2i(glGetUniformLocation(ID,  varName), vec2.x, vec2.y);
}

void Shader::uploadFloat(const char* varName, float value) const
{
    glUniform1f(glGetUniformLocation(ID,  varName), value);
}

void Shader::uploadInt(const char* varName, int value) const
{
    glUniform1i(glGetUniformLocation(ID,  varName), value);
}

void Shader::uploadUInt(const char* varName, unsigned int value) const
{
    glUniform1ui(glGetUniformLocation(ID,  varName), value);
}

void Shader::uploadIntArray(const char* varName, int length, const int* array) const
{
    glUniform1iv(glGetUniformLocation(ID,  varName), length, array);
}

void Shader::uploadBool(const char* varName, bool value) const
{
    glUniform1i(glGetUniformLocation(ID,  varName), value ? 1 : 0);
}

void Shader::uploadMat4(const char* varName, const glm::mat4& mat4) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID,  varName), 1, GL_FALSE, glm::value_ptr(mat4));
}

void Shader::uploadMat3(const char* varName, const glm::mat3& mat3) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID,  varName), 1, GL_FALSE, glm::value_ptr(mat3));
}


// error checking
void Shader::compileErrors(unsigned int shader, const char* type){
     // boolean to make sure to only check if it didn't compile
     GLint hasCompiled;
     // space for the log
     char infoLog[1024];
     // if was program we want to check for linking errors
     if (type != "PROGRAM"){
            // get if even has compiled
            glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
            if (hasCompiled == GL_FALSE){
             // get da info
             glGetShaderInfoLog(shader, 1024, NULL, infoLog);
             std::cout << infoLog << std::endl;
             std::cout << "[ERROR]: SHADER_COMPILATION_ERROR for: " << type << "\n" << std::endl;
            }
     }
     else{
          // this time its for the program
          glGetProgramiv(shader, GL_COMPILE_STATUS, &hasCompiled);
          if (hasCompiled == GL_FALSE){
           // put it into infoLog
           glGetProgramInfoLog(shader, 1024, NULL, infoLog);
           std::cout << infoLog << std::endl;
           std::cout << "[ERROR]: SHADER_LINKING_ERROR for: " << type << "\n" << std::endl;
          }
     }
}