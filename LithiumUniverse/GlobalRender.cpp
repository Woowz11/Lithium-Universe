#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>
#include <GLM/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <windows.h>
#include <iostream>
#include <chrono>

#include "ExplorerActions.h";
#include "GlobalPhysic.h";
#include "GameCamera.h";
#include "GameObject.h";
#include "GameData.h";
#include "Console.h";
#include "Shader.h";

uint32_t START_WINDOW_WIDTH;
uint32_t START_WINDOW_HEIGHT;
uint32_t CURRENT_WINDOW_WIDTH;
uint32_t CURRENT_WINDOW_HEIGHT;
void UpdateWindowSize(uint32_t W, uint32_t H) {
    CURRENT_WINDOW_WIDTH  = W;
    CURRENT_WINDOW_HEIGHT = H;
}

LARGE_INTEGER AppTimeFrequency, AppTimeStart, AppTimeEnd;

std::string GamePath;

float DeltaTime = 0;
float Time = 0;
void UpdateDeltaTime(float DT) {
    Camera->SetDeltaTime(DT);
    DeltaTime = DT;
}

/* ==== Глобальное ==== */

/* Цвет заднего фона */
glm::vec3 BackgroundColor = glm::vec3(0.05f, 0.05f, 0.05f);

/* ==== Вертиксы ==== */

unsigned int VBO, VAO;

float square[] = {
    -0.5f, -0.5f, -0.5f,    0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f
};
int square_l = 4; /* Кол-во строк в square */

/* ==== Шейдеры ==== */

std::vector<Shader> Shaders = {};

/* Информация о uniform's
Projection   (mat4)      = Проекция (от камеры)
Position     (mat4)      = Позиция объекта
LinePosition (mat4)      = Позиция начала и конца линии
Random       (float)     = Случайное дробное число от 0 до 1
Texture      (sampler2D) = Текстура
Time         (float)     = Прошедшее время с запуска приложения
DeltaTime    (float)     = Размягчение зависящие от FPS

Информация о location's
[0] PolygonPosition (vec3) = Позиция полигона
[1] TextureUV       (vec2) = Развёртка текстуры
*/

Shader DefaultShader;

std::string DefaultShader_Vert = R"(#version 330 core
layout (location = 0) in vec3 PolygonPosition;
layout (location = 1) in vec2 TextureUV;

out vec2 TextureCoord;

uniform mat4 Position;
uniform mat4 Projection;
uniform float Random;
uniform float Time;
uniform float DeltaTime;

void main()
{
    gl_Position = Projection * Position * vec4(PolygonPosition, 1.0f);
    TextureCoord = vec2(TextureUV.x, 1.0 - TextureUV.y);
})";

std::string DefaultShader_Frag = R"(#version 330 core
out vec4 FragColor;

in vec2 TextureCoord;

uniform sampler2D Texture;
uniform vec4 Color;
uniform float Random;
uniform float Time;
uniform float DeltaTime;

void main()
{
    vec4 TextureColor = texture(Texture, TextureCoord) * Color;
    if(TextureColor.a == 0){ discard; }
    FragColor = TextureColor;
})";

Shader ErrorShader;

std::string ErrorShader_Frag = R"(#version 330 core
out vec4 FragColor;

in vec2 TextureCoord;
uniform vec4 Color;

uniform sampler2D Texture;

void main()
{
    FragColor = vec4(1.0f,0.0f,1.0f,1.0f) * Color;
})";

Shader LineShader;

std::string LineShader_Vert = R"(#version 330 core
layout (location = 0) in vec3 PolygonPosition;
layout (location = 1) in vec2 TextureUV;

out vec2 TextureCoord;

uniform mat4 LinePosition;
uniform mat4 Projection;
uniform float Random;
uniform float Time;
uniform float DeltaTime;

void main()
{
    gl_Position = Projection * LinePosition * vec4(PolygonPosition, 1.0f);
    TextureCoord = vec2(TextureUV.x, 1.0 - TextureUV.y);
})";

/* Создать шейдеры */
void CreateShaders() {
    ErrorShader = *new Shader("Error", DefaultShader_Vert, ErrorShader_Frag);
    Shaders.push_back(ErrorShader);
    DefaultShader = *new Shader("Default", DefaultShader_Vert, DefaultShader_Frag);
    Shaders.push_back(DefaultShader);
    LineShader = *new Shader("Line", LineShader_Vert, DefaultShader_Frag);
    Shaders.push_back(LineShader);
}

/* ==== Текстуры ==== */

unsigned int DefaultTexture;
unsigned int Default2Texture;
unsigned int CircleTexture;
unsigned int CableTexture;

/* Создать текстуру */
void CreateTexture(std::string Path, unsigned int& Texture) {
    if (HasFile(Path)) {
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST /*GL_LINEAR*/);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /*GL_LINEAR*/);

        int width, height, channels;
        unsigned char* data = stbi_load(Path.c_str(), &width, &height, &channels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, channels==4? GL_RGBA: GL_RGB, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            Print("TEXTURE", "Texture ($$Y" + Path + "$$_ ($$B" + std::to_string(Texture) + "$$_)) $$Gcreated$$_!");
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
    CreateTexture(AddFileToPath(VanillaTexturesFolder, "Default2.png"), Default2Texture);
    CreateTexture(AddFileToPath(VanillaTexturesFolder, "Circle.png"), CircleTexture);
    CreateTexture(AddFileToPath(VanillaTexturesFolder, "Cable.png"), CableTexture);
}

/* Установить всё для рендера */
void InstallRender(std::string GamePath_ ,uint32_t SWW, uint32_t SWH) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GamePath = GamePath_;
    START_WINDOW_WIDTH = SWW;
    START_WINDOW_HEIGHT = SWH;

    QueryPerformanceFrequency(&AppTimeFrequency);
    QueryPerformanceCounter(&AppTimeStart);

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

    glUseProgram(DefaultShader.ID);
    DefaultShader.setInt("Texture", 0);

    /* ==== Физика ==== */

    InstallPhysic();
}

/* Удалить всё что осталось после рендера */
void ClearRender() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

/* ==== Рендер объектов ==== */
Shader CSS;

/* Рендер квадрата */
void RenderSquare(const GameObject& OBJ) {
    /* ==== Трансформация ==== */
    glm::mat4 Projection = glm::mat4(1.0f);
    float Zoom = 1 / (OBJ.ThatUI ? 1 : Camera->Zoom);

    float WIN_WIDTH = OBJ.Resize ? (float)START_WINDOW_WIDTH : (float)CURRENT_WINDOW_WIDTH;
    float WIN_HEIGHT = OBJ.Resize ? (float)START_WINDOW_HEIGHT : (float)CURRENT_WINDOW_HEIGHT;

    float Left = -WIN_WIDTH / 240;
    float Right = WIN_WIDTH / 240;
    float Down = -WIN_HEIGHT / 240;
    float Up = WIN_HEIGHT / 240;
    Projection = glm::ortho(
        Left / Zoom,
        Right / Zoom,
        Down / Zoom,
        Up / Zoom,
        -1000.0f, 1000.0f);
    CSS.setMat4("Projection", Projection);

    glBindVertexArray(VAO);

    glm::mat4 ResultPosition = glm::mat4(1.0f);

    if (!OBJ.ThatUI) {
        ResultPosition = glm::rotate(ResultPosition, -glm::radians(Camera->Rotation), glm::vec3(0, 0, 1));
    }

    ResultPosition = glm::translate(ResultPosition, glm::vec3(OBJ.Position, OBJ.Layer + (float)OBJ.GetID() / 10000));
    if (!OBJ.ThatUI) {
        ResultPosition = glm::translate(ResultPosition, glm::vec3(Camera->Position.x, Camera->Position.y, 0));
    }

    ResultPosition = glm::rotate(ResultPosition, -glm::radians(OBJ.Orientation), glm::vec3(0, 0, 1));
    ResultPosition = glm::scale(ResultPosition, glm::vec3(OBJ.Size, 1));

    CSS.setMat4("Position", ResultPosition);

    glDrawArrays(GL_QUADS, 0, square_l);
}

/* Рендерить линию */
void RenderLine(const GameObject& OBJ) {
    /* ==== Трансформация ==== */
    glm::mat4 Projection = glm::mat4(1.0f);
    float Zoom = 1 / (OBJ.ThatUI ? 1 : Camera->Zoom);

    float WIN_WIDTH = OBJ.Resize ? (float)START_WINDOW_WIDTH : (float)CURRENT_WINDOW_WIDTH;
    float WIN_HEIGHT = OBJ.Resize ? (float)START_WINDOW_HEIGHT : (float)CURRENT_WINDOW_HEIGHT;

    float Left = -WIN_WIDTH / 240;
    float Right = WIN_WIDTH / 240;
    float Down = -WIN_HEIGHT / 240;
    float Up = WIN_HEIGHT / 240;
    Projection = glm::ortho(
        Left / Zoom,
        Right / Zoom,
        Down / Zoom,
        Up / Zoom,
        -1000.0f, 1000.0f);
    CSS.setMat4("Projection", Projection);

    glBindVertexArray(VAO);

    float Thickness = OBJ.Size.x;
    glm::vec2 StartPos = glm::vec2(OBJ.LinePosition.x, OBJ.LinePosition.y);
    glm::vec2 EndPos = glm::vec2(OBJ.LinePosition.z, OBJ.LinePosition.w);
    glm::vec2 CenterPos = StartPos / glm::vec2(2, 2) + EndPos / glm::vec2(2, 2);

    glm::mat4 ResultPosition = glm::mat4(1.0f);
    
    glm::vec2 Direction = EndPos - StartPos;
    float rad = atan2(Direction.y, Direction.x) - glm::half_pi<float>();;

    if (!OBJ.ThatUI) {
        ResultPosition = glm::rotate(ResultPosition, -glm::radians(Camera->Rotation), glm::vec3(0, 0, 1));
    }

    ResultPosition = glm::translate(ResultPosition, glm::vec3(CenterPos.x, CenterPos.y, OBJ.Layer + (float)OBJ.GetID() / 10000));

    if (!OBJ.ThatUI) {
        ResultPosition = glm::translate(ResultPosition, glm::vec3(Camera->Position.x, Camera->Position.y, 0));
    }

    ResultPosition = glm::rotate(ResultPosition, rad, glm::vec3(0, 0, 1));
    ResultPosition = glm::scale(ResultPosition, glm::vec3(Thickness, glm::distance(StartPos, EndPos), 1));

    CSS.setMat4("LinePosition", ResultPosition);

    glDrawArrays(GL_QUADS, 0, square_l);
}

/* Рендер картинки каждый кадр */
void Render(std::vector<GameObject>& Scene) {
	glClearColor(BackgroundColor.r, BackgroundColor.g, BackgroundColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Time = static_cast<double>(AppTimeEnd.QuadPart - AppTimeStart.QuadPart) / AppTimeFrequency.QuadPart;

    /* ==== Рендер объектов ==== */
    for (const GameObject& OBJ : Scene) {
        if (OBJ.Active && OBJ.Render) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, OBJ.BaseTexture);

            Shader OBJ_Shader = Shaders[OBJ.BaseShader];
            if (CSS.RealID != OBJ_Shader.RealID) {
                CSS = OBJ_Shader;
                glUseProgram(CSS.ID);
            }

            CSS.setFloat("Random", (static_cast<double>(rand()) / RAND_MAX));

            QueryPerformanceCounter(&AppTimeEnd);
            CSS.setFloat("Time", Time);

            CSS.setVec4("Color", OBJ.Color);

            switch (OBJ.Shape)
            {
            case ST_Line:
                RenderLine(OBJ);
                break;
            default:
                RenderSquare(OBJ);
                break;
            }
        }
    }
}

/* Рендер и обновление физики */
void RenderAndPhysic() {
    Render(UpdatePhysic());
}

/* Позицию экранную в мировую */
glm::vec2 ScreenPositionToWorld(glm::vec2 Pos) {
    glm::vec2 Result = Pos - glm::vec2(0.5f, 0.5f);
    //800px => 20.0f / 3
    Result *= glm::vec2(20.0f / 3, 15.0f / -3);
    Result *= Camera->Zoom;
    Result = glm::rotate(Result, glm::radians(Camera->Rotation));
    Result -= Camera->Position;
    return Result;
}

/* Курсор двигается */
void MouseRenderMove(glm::vec2 Pos, glm::vec2 Pos2) {
    UpdateMousePhysic(Pos, Pos2);
}