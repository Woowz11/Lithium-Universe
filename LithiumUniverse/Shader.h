#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#include "Console.h";
#include "StringActions.h";
#include "GlobalResources.h";

class Shader
{
public:
    unsigned int ID;
    unsigned int RealID;
    std::string FullPath;
    std::string Path = "";
    bool Success = true;

    Shader() { RealID = -1; ID = -1; FullPath = ""; }

    Shader(std::string FullPath_, std::string VertexCode, std::string FragmentCode, int ErrorShaderID)
    {
        FullPath = FullPath_;
        Path = FullPathToComplex(FullPath_);

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

        if (Success) {
            ID = RealID;
        }
        else {
            ID = ErrorShaderID;
        }

        Print("SHADER", "Shader ($$Y" + Path + "$$_ ($$B" + std::to_string(RealID) + "$$_)) " + (Success ? "$$Gcreated" : "has $$Rerrors") + "$$_!");

        glUseProgram(ID);
        UpdateUniforms();
    }

    void DeleteShader() {
        if (RealID != -1) {
            glDeleteProgram(RealID);
        }
    }

    GLint UNIFORM_POSITION;
    GLint UNIFORM_ID;
    GLint UNIFORM_ORIENTATION;
    GLint UNIFORM_SIZE;
    GLint UNIFORM_CENTER;
    GLint UNIFORM_LAYER;
    GLint UNIFORM_STATIC;
    GLint UNIFORM_PHYSICAL;
    GLint UNIFORM_INTERFACE;
    GLint UNIFORM_RESIZE;

    GLint UNIFORM_TEXTLENGTH;
    GLint UNIFORM_TEXTWIDTH;

    GLint UNIFORM_TEXTURESIZE;
    GLint UNIFORM_SCREENSTARTSIZE;
    GLint UNIFORM_SCREENSIZE;
    GLint UNIFORM_MOUSEPOSITION;
    GLint UNIFORM_CAMERAPOSITION;
    GLint UNIFORM_CAMERAORIENTATION;
    GLint UNIFORM_CAMERAZOOM;
    GLint UNIFORM_RANDOM;
    GLint UNIFORM_LOCALRANDOM;
    GLint UNIFORM_TIME;
    GLint UNIFORM_COLOR;
    GLint UNIFORM_DEBUGRENDER;

    void UpdateUniforms() {
        UNIFORM_POSITION    = glGetUniformLocation(ID, "Position");
        UNIFORM_ID          = glGetUniformLocation(ID, "ID");
        UNIFORM_ORIENTATION = glGetUniformLocation(ID, "Orientation");
        UNIFORM_SIZE        = glGetUniformLocation(ID, "Size");
        UNIFORM_CENTER      = glGetUniformLocation(ID, "Center");
        UNIFORM_LAYER       = glGetUniformLocation(ID, "Layer");
        UNIFORM_STATIC      = glGetUniformLocation(ID, "Static");
        UNIFORM_PHYSICAL    = glGetUniformLocation(ID, "Physical");
        UNIFORM_INTERFACE   = glGetUniformLocation(ID, "Interface");
        UNIFORM_RESIZE      = glGetUniformLocation(ID, "Resize");

        UNIFORM_TEXTLENGTH = glGetUniformLocation(ID, "TextLength");
        UNIFORM_TEXTWIDTH = glGetUniformLocation(ID, "TextWidth");

        UNIFORM_TEXTURESIZE       = glGetUniformLocation(ID, "TextureSize");
        UNIFORM_SCREENSTARTSIZE   = glGetUniformLocation(ID, "ScreenStartSize");
        UNIFORM_SCREENSIZE        = glGetUniformLocation(ID, "ScreenSize");
        UNIFORM_MOUSEPOSITION     = glGetUniformLocation(ID, "MousePosition");
        UNIFORM_CAMERAPOSITION    = glGetUniformLocation(ID, "CameraPosition");
        UNIFORM_CAMERAORIENTATION = glGetUniformLocation(ID, "CameraOrientation");
        UNIFORM_CAMERAZOOM        = glGetUniformLocation(ID, "CameraZoom");
        UNIFORM_RANDOM            = glGetUniformLocation(ID, "Random");
        UNIFORM_LOCALRANDOM       = glGetUniformLocation(ID, "LocalRandom");
        UNIFORM_TIME              = glGetUniformLocation(ID, "Time");
        UNIFORM_COLOR             = glGetUniformLocation(ID, "Color");
        UNIFORM_DEBUGRENDER       = glGetUniformLocation(ID, "DebugRender");
    }

    void setBool(const GLint& id, bool value) const
    {
        glUniform1i(id, (int)value);
    }
    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const GLint& id, int value) const
    {
        glUniform1i(id, value);
    }
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    void setFloat(const GLint& id, float value) const
    {
        glUniform1f(id, value);
    }

    void setVec2(const GLint& id, const glm::vec2& value) const
    {
        glUniform2fv(id, 1, &value[0]);
    }
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec3(const GLint& id, const glm::vec3& value) const
    {
        glUniform3fv(id, 1, &value[0]);
    }
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec4(const GLint& id, const glm::vec4& value) const
    {
        glUniform4fv(id, 1, &value[0]);
    }
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
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
                Error("SHADER (COMPIL.)", "Error with shader ($$Y" + FullPath + "$$_ type: $$Y" + type + "$$_ ($$P" + std::to_string(shader) + "$$_)");
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
                Error("SHADER (LINKING)", "Error with shader ($$Y" + FullPath + "$$_ ($$B" + std::to_string(RealID) + "$$_)) type: $$Y" + type + "$$_ ($$P" + std::to_string(shader) + "$$_)");
                Error("SHADER (LINKING)", RemoveLastSymbol(infoLog));
                Error("SHADER (LINKING)", "$$R---------------------------------------------------");
            }
        }
        if (!success) {
            Success = false;
        }
    }
};

extern std::vector<Shader> Shaders;

#endif