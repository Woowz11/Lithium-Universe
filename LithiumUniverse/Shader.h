#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <sstream>
#include <iostream>

#include "StringActions.h";
#include "Console.h";

static unsigned int ErrorShaderID = 3;

class Shader
{
public:
    unsigned int ID;
    unsigned int RealID;
    std::string Name;
    bool Success = true;

    Shader() { RealID = -1; ID = ErrorShaderID; Name = "Unknown"; }

    Shader(std::string Name_, std::string VertexCode, std::string FragmentCode)
    {
        Name = Name_;

        std::string vertexCode;
        std::string fragmentCode;

        const char* vShaderCode = VertexCode.c_str();
        const char* fShaderCode = FragmentCode.c_str();

        unsigned int vertex, fragment;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        RealID = glCreateProgram();
        glAttachShader(RealID, vertex);
        glAttachShader(RealID, fragment);
        glLinkProgram(RealID);
        checkCompileErrors(RealID, "PROGRAM");

        glDeleteShader(vertex);
        glDeleteShader(fragment);

        Print("SHADER", "Shader ($$Y" + Name + "$$_ ($$B" + std::to_string(RealID) + "$$_)) "+(Success?"$$Gcreated" : "has $$Rerrors") + "$$_!");
        if (Success) {
            ID = RealID;
        }
        else {
            ID = ErrorShaderID;
        }
    }

    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }

    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }

    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

private:
    void checkCompileErrors(unsigned int shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                Error("SHADER (COMPIL.)", "Error with shader ($$Y" + Name + "$$_ ($$B" + std::to_string(RealID) + "$$_)) type: $$C" + type);
                Error("SHADER (COMPIL.)", RemoveLastSymbol(infoLog));
                Error("SHADER (COMPIL.)", "$$R---------------------------------------------------");
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                Error("SHADER (LINKING)", "Error with shader ($$Y" + Name + "$$_ ($$B" + std::to_string(RealID) + "$$_)) type: $$C" + type);
                Error("SHADER (LINKING)", RemoveLastSymbol(infoLog));
                Error("SHADER (LINKING)", "$$R---------------------------------------------------");
            }
        }
        if (!success) {
            Success = false;
        }
    }
};
#endif