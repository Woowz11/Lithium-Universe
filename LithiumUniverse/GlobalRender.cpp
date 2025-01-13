#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "ExplorerActions.h";
#include "RenderedObject.h";
#include "Console.h";
#include "Shader.h";

uint32_t START_WINDOW_WIDTH;
uint32_t START_WINDOW_HEIGHT;
std::string GamePath;

/* ==== Камера ==== */

glm::vec2 CameraPosition = glm::vec2(0, 0);
float CameraSpeed = 0.001f;
float CameraZoomSpeed = 0.001f;
float CameraZoom = 1;

/* Двигать камеру */
void MoveCamera(float vel_x, float vel_y) {
    CameraPosition = CameraPosition + glm::vec2(-vel_x * CameraSpeed * CameraZoom, -vel_y * CameraSpeed * CameraZoom);
}

/* Установить позицию камере */
void SetCameraPosition(float x, float y) {
    CameraPosition = glm::vec2(x, y);
}

/* Двигать масштаб камеры */
void MoveCameraZoom(float vel) {
    CameraZoom += (-vel * CameraZoomSpeed) * CameraZoom;
}

/* Изменить масштаб камеры */
void SetCameraZoom(float z) {
    CameraZoom = z;
}

/* ==== Вертиксы ==== */

unsigned int VBO, VAO;

float square[] = {
    -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,
     1.0f, -1.0f, -1.0f,    1.0f, 0.0f,
     1.0f,  1.0f, -1.0f,    1.0f, 1.0f,
    -1.0f,  1.0f, -1.0f,    0.0f, 1.0f
};
int square_l = 4; /* Кол-во строк в square */

/* ==== Шейдеры ==== */

Shader DefaultShader;

std::string DefaultShader_Vert = R"(#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Random;

void main()
{
    gl_Position = Projection * View * Model * vec4(aPos, 1.0f);
    TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
})";

std::string DefaultShader_Frag = R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Texture;
uniform float Random;

void main()
{
    FragColor = texture(Texture, TexCoord);
})";

Shader ErrorShader;

std::string ErrorShader_Frag = R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D Texture;

void main()
{
    FragColor = vec4(1.0f,0.0f,1.0f,1.0f);
})";

/* Создать шейдеры */
void CreateShaders() {
    ErrorShader = *new Shader("Error", DefaultShader_Vert, ErrorShader_Frag);
    DefaultShader = *new Shader("Default", DefaultShader_Vert, DefaultShader_Frag);
}

/* ==== Текстуры ==== */

unsigned int DefaultTexture;

/* Создать текстуру */
void CreateTexture(std::string Path, unsigned int& Texture) {
    if (HasFile(Path)) {
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST /*GL_LINEAR*/);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /*GL_LINEAR*/);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(Path.c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            Print("TEXTURE", "Texture ($$Y" + Path + "$$_ ($$G" + std::to_string(Texture) + "$$_)) created!");
        }
        else
        {
            Error("TEXTURE", "Failed to load texture! Path: " + Path);
        }
        stbi_image_free(data);
    }
    else {
        Error("TEXTURE", "The required texture was not found! Path: " + Path);
        Texture = -1;
    }
}

/* Создать текстуры */
void CreateTextures() {
    std::string VanillaTexturesFolder = AddFileToPath(AddFileToPath(GamePath, "Resources"), "Textures");
    CreateFolder(VanillaTexturesFolder);

    CreateTexture(AddFileToPath(VanillaTexturesFolder, "Default.png"), DefaultTexture);
}

/* ==== Сцена ==== */

std::vector<RenderedObject> Scene = {};

void InstallRender(std::string GamePath_ ,uint32_t SWW, uint32_t SWH) {
    GamePath = GamePath_;
    START_WINDOW_WIDTH = SWW;
    START_WINDOW_HEIGHT = SWH;

    /* ==== Шейдеры ====*/

    CreateShaders();

    /* ==== Вертиксы ====*/

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /* ==== Текстуры ==== */

    CreateTextures();

    DefaultShader.use();
    DefaultShader.setInt("Texture", 0);
}

void ClearRender() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Render() {
	glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, DefaultTexture);

    DefaultShader.use();

    /* ==== Трансформация ==== */
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    float Zoom  = 1/CameraZoom;
    float Left  = -(float)START_WINDOW_WIDTH / 240;
    float Right = (float)START_WINDOW_WIDTH / 240;
    float Down  = -(float)START_WINDOW_HEIGHT / 240;
    float Up    = (float)START_WINDOW_HEIGHT / 240;
    projection  = glm::ortho(Left/Zoom, Right/Zoom, Down/Zoom, Up/Zoom, -1000.0f, 1000.0f); //glm::perspective(glm::radians(45.0f), (float)START_WINDOW_WIDTH / (float)START_WINDOW_HEIGHT, 0.1f, 100.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    DefaultShader.setMat4("Projection", projection);
    DefaultShader.setMat4("View", view);

    glBindVertexArray(VAO);

    double randomValue = ((static_cast<double>(rand()) / RAND_MAX) - 0.5f) * 2;

    DefaultShader.setFloat("Random", randomValue);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(CameraPosition.x, CameraPosition.y, 0));
    DefaultShader.setMat4("Model", model);

    glDrawArrays(GL_QUADS, 0, square_l);
}