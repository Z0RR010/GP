#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
//#include "Building.h"
#include <glm/gtc/type_ptr.hpp>
using namespace std;
struct Light {
    int type;          // 0: Directional, 1: Point, 2: Spotlight
    bool on;           // Enable or disable this light
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float intensity;
    float cutoff;       // For spotlights
    float outerCutoff;  // For spotlights
    bool rotate = false;
    float speed = 0.2f;
    float radius = 6;
    bool draw = true;
};

class Shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void setLight(int index, const Light light) const
    {
        std::string base = "lights[" + std::to_string(index) + "]";
        //cout << light.color[0] << endl;
        //cout << glGetUniformLocation(ID, (base + ".color").c_str()) << endl;
        glUniform1i(glGetUniformLocation(ID, (base + ".type").c_str()), light.type);
        glUniform1i(glGetUniformLocation(ID, (base + ".on").c_str()), (int)light.on);
        glUniform3fv(glGetUniformLocation(ID, (base + ".position").c_str()), 1, &(light.position[0]));
        glUniform3fv(glGetUniformLocation(ID, (base + ".direction").c_str()), 1, &(light.direction[0]));
        glUniform3fv(glGetUniformLocation(ID, (base + ".color").c_str()), 1, &(light.color[0]));
        /*glUniform3fv(glGetUniformLocation(ID, (base + ".position").c_str()), 1, glm::value_ptr(light.position));
        glUniform3fv(glGetUniformLocation(ID, (base + ".direction").c_str()), 1, glm::value_ptr(light.direction));
        glUniform3fv(glGetUniformLocation(ID, (base + ".color").c_str()), 1, glm::value_ptr(light.color));*/
        glUniform1f(glGetUniformLocation(ID, (base + ".intensity").c_str()), light.intensity);
        glUniform1f(glGetUniformLocation(ID, (base + ".cutoff").c_str()), light.cutoff);
        glUniform1f(glGetUniformLocation(ID, (base + ".outerCutoff").c_str()), light.outerCutoff);
    }
};

#endif