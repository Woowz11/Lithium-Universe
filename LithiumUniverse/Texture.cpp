#include <glad/glad.h>
#include <stb_image.h>

#include <string>
#include <vector>

#include "DebugGetter.h";
#include "Texture.h";
#include "ExplorerActions.h";
#include "Console.h";

/* Все текстуры */
std::vector<Texture> Texturies = {};

int NotSelectedTexture = 1;

/* Создать текстуру */
void CreateTexture(std::string Path, std::string Atlas) {
    if (HasFile(Path)) {
        Texture info = Texture(Path, Atlas);

        glGenTextures(1, &info.ID);
        glBindTexture(GL_TEXTURE_2D, info.ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, info.Blur ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, info.Blur ? GL_LINEAR : GL_NEAREST);

        int width, height, channels;
        unsigned char* data = stbi_load(info.FullPath.c_str(), &width, &height, &channels, 0);
        if (data)
        {
            info.HasAlpha = channels == 4;
            info.Width = width;
            info.Height = height;
            glTexImage2D(GL_TEXTURE_2D, 0, info.HasAlpha ? GL_RGBA : GL_RGB, width, height, 0, info.HasAlpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            Print("TEXTURE", "Texture ($$Y" + info.FullPath + "$$_ ($$B" + std::to_string(info.ID) + "$$_)) $$Gcreated$$_!");
            DebugLog_LoadTexture(info, true);
        }
        else
        {
            Error("TEXTURE", "Failed to load texture! Path: " + info.FullPath);
            info.Error = true;
            DebugLog_LoadTexture(info, false);
        }
        stbi_image_free(data);

        Texturies.push_back(info);
    }
    else {
        Error("TEXTURE", "The required texture was not found! Path: " + Path);
    }
}