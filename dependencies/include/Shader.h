#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>


class Shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(std::string vertexFile, std::string fragmentFile);
    Shader(std::string vertexFile);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    //Custom uniform
    void setVec2(const std::string& name, glm::vec2 values) const;
    void setVec3(const std::string& name, std::vector<float> values) const;
    void setVec3(const std::string& name, glm::vec3 values) const;
    void setVec3(const std::string& name, float values[3]) const;
    void setMat4(const std::string& name, glm::mat4 transformMatrix);
    
};

//Read filename to string
extern void readFromFile(std::string filename, std::string& data);
//Compile Shaders
extern unsigned int& compileShader(GLenum shaderType, const char* shaderSource);

#endif