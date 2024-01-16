#ifndef SHADER_H
#define SHADER_H

#include "glad.h"
#include "glm/glm.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderProgram
{
public:
    unsigned int glid;

    ShaderProgram(const char* vertexPath, const char* fragmentPath)
    {
        std::string vsCode;
        std::string fsCode;
        std::ifstream vsFile;
        std::ifstream fsFile;
        vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try 
        {
            // open files
            vsFile.open(vertexPath);
            fsFile.open(fragmentPath);
            std::stringstream vsStream, fsStream;
            // read file's buffer contents into streams
            vsStream << vsFile.rdbuf();
            fsStream << fsFile.rdbuf();	
            // close file handlers
            vsFile.close();
            fsFile.close();
            // convert stream into string
            vsCode = vsStream.str();
            fsCode = fsStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "READING SHADER FAILED! Error: " << e.what() << std::endl;
            exit(0);
        }

        std::string test1 = "#version 330 core \nlayout (location = 0) in vec3 pos; \nuniform mat4 perspective;\nuniform mat4 view; \nvoid main(){gl_Position = perspective*view*vec4(pos, 1.0);}";
        std::string test2 = "#version 330 core\nout vec4 fragCol; \nvoid main(){fragCol = vec4(1.0);}";
        const char *vShaderCode = vsCode.c_str();
        const char *fShaderCode = fsCode.c_str();

        // 2. compile shaders
        unsigned int vertex, fragment;

        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        check_compile_errors(vertex, "VERTEX");

        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        check_compile_errors(fragment, "FRAGMENT");

        // create shader pgrm
        glid = glCreateProgram();
        glAttachShader(glid, vertex);
        glAttachShader(glid, fragment);

        glLinkProgram(glid);
        check_compile_errors(glid, "PROGRAM");

        // delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);


    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() 
    { 
        glUseProgram(glid); 
    }


    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const
    { 
        float loc = glGetUniformLocation(glid, name.c_str());
        if (loc != -1.0) glUniform1i(loc, value); 
    }

    void setFloat(const std::string &name, float value) const
    {   
        float loc = glGetUniformLocation(glid, name.c_str());
        if (loc != -1.0) glUniform1f(loc, value);
    }

    void setMat4(const std::string &name, const glm::mat4 &value)
    {
        float loc = glGetUniformLocation(glid, name.c_str());
        if (loc != -1.0) glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
    }

    void setVec3(const std::string &name, const glm::vec3 &value)
    {
        float loc = glGetUniformLocation(glid, name.c_str());
        if (loc != -1.0) glUniform3fv(loc, 1, &value[0]);
    }


private:

    void check_compile_errors(GLuint shader, std::string type)
    {
        GLint success;
        if(type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success)
            {
                int infoLength;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

                GLchar *log = new char[infoLength];
                glGetShaderInfoLog(shader, infoLength, NULL, log);

                std::cout << "COMPILING SHADER FAILED! ERROR type: " << type << "\n" << log << "\n ---- " << std::endl;
                delete[] log;

            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success)
            {
                int infoLength;
                glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &infoLength);

                GLchar *log = new char[infoLength];
                glGetProgramInfoLog(shader, infoLength, NULL, log);
                std::cout << "LINKING PROGRAM FAILED! of type: " << type << "\n" << log << "\n ---- " << std::endl;

                delete[] log;
            }
        }
    }
};
#endif