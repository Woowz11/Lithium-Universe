#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "Console.h";
#include "Shader.h";

uint32_t START_WINDOW_WIDTH;
uint32_t START_WINDOW_HEIGHT;

unsigned int VBO, VAO;

Shader ourShader;

unsigned int texture1;

int vertices_length = 4; /* Кол-во строк в vertices */
float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

glm::vec2 CameraPosition = glm::vec2(0,0);
float CameraSpeed = 0.001f;

void MoveCamera(float vel_x, float vel_y) {
    CameraPosition = CameraPosition + glm::vec2(-vel_x * CameraSpeed, -vel_y * CameraSpeed);
}

void SetCameraPosition(float x, float y) {
    CameraPosition = glm::vec2(x,y);
}

void InstallRender(uint32_t SWW, uint32_t SWH) {
    START_WINDOW_WIDTH = SWW;
    START_WINDOW_HEIGHT = SWH;

    ourShader = *new Shader("F:/Lithium-Universe/Resources/shader.vert", "F:/Lithium-Universe/Resources/shader.frag");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    /* ТЕКСТУРА */

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST /*GL_LINEAR*/);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /*GL_LINEAR*/);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("F:/Lithium-Universe/Resources/texture.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    ourShader.use();
    ourShader.setInt("texture1", 0);
}

void ClearRender() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Render() {
	glClearColor(0.2f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    ourShader.use();

    /* Трансформация */
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)START_WINDOW_WIDTH / (float)START_WINDOW_HEIGHT, 0.1f, 100.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);

    glBindVertexArray(VAO);

    double randomValue = ((static_cast<double>(rand()) / RAND_MAX) - 0.5f) * 2;
    double randomValue2 = ((static_cast<double>(rand()) / RAND_MAX) - 0.5f) * 2;

    ourShader.setFloat("random", randomValue);
    ourShader.setFloat("random2", randomValue2);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(CameraPosition.x, CameraPosition.y, 0));
    ourShader.setMat4("model", model);

    glDrawArrays(GL_QUADS, 0, vertices_length);
}