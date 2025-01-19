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

#include "GameObjectActions.h";
#include "ExplorerActions.h";
#include "GlobalPhysic.h";
#include "GameCamera.h";
#include "GameObject.h";
#include "GameData.h";
#include "Texture.h";
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

/* Путь до игры */
std::string GamePath;

/* Константа на размер окна */
const glm::vec2 ScreenScale    = glm::vec2(10.0f / 3, 7.5f / 3);

/* Отладочный рендер */
const bool DebugRender = false;

/* ==== Глобальное ==== */

/* Цвет заднего фона */
glm::vec3 BackgroundColor = glm::vec3(0.1f, 0.1f, 0.1f);

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

std::vector<Shader> Shaders = {};

/* Информация о uniform's
Projection   (mat4     ) = Проекция (от камеры)
Position     (mat4     ) = Позиция объекта
LinePosition (mat4     ) = Позиция начала и конца линии
Random       (float    ) = Случайное дробное число от 0 до 1
Texture      (sampler2D) = Текстура
Time         (float    ) = Прошедшее время с запуска приложения
DeltaTime    (float    ) = Размягчение зависящие от FPS
DebugRender  (bool     ) = Включен отладочный рендер?
Sleeping     (bool     ) = Физическое тело объекта спит?

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

uniform bool DebugRender;
uniform bool Sleeping;

void main()
{
    vec4 TextureColor = texture(Texture, TextureCoord) * Color;
    if(TextureColor.a == 0){ discard; }
    if(DebugRender){
        if(Sleeping){
            TextureColor *= vec4(1,0,0,1);
        }
    }
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

/* Создать текстуры */
void CreateTextures() {
    std::string VanillaTexturesFolder = AddFileToPath(AddFileToPath(GamePath, "Resources"), "Textures");
    CreateFolder(VanillaTexturesFolder);

    CreateTexture(AddFileToPath(VanillaTexturesFolder, "NotSelected.png"), "Base"   );
    CreateTexture(AddFileToPath(VanillaTexturesFolder, "Default.png"    ), "Vanilla");
    CreateTexture(AddFileToPath(VanillaTexturesFolder, "Default2.png"   ), "Vanilla");
    CreateTexture(AddFileToPath(VanillaTexturesFolder, "Circle.png"     ), "Vanilla");
    CreateTexture(AddFileToPath(VanillaTexturesFolder, "Cable.png"      ), "Vanilla");
}

/* Установить всё для рендера */
void InstallRender(std::string GamePath_ ,uint32_t SWW, uint32_t SWH, bool DV) {
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

    InstallPhysic(DV);
}

/* Удалить всё что осталось после рендера */
void ClearRender() {
    ClearPhysic();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

/* ==== Рендер объектов ==== */
Shader CSS;
int TEX = -2;

/* Рендер квадрата */
void RenderSquare(const GameObject& OBJ) {
    /* ==== Трансформация ==== */
    glm::mat4 Projection = glm::mat4(1.0f);
    float Zoom = 1 / (OBJ.Type == RO_UI ? 1 : Camera->Zoom);

    float WIN_WIDTH  = OBJ.Resize ? (float)START_WINDOW_WIDTH  : (float)CURRENT_WINDOW_WIDTH;
    float WIN_HEIGHT = OBJ.Resize ? (float)START_WINDOW_HEIGHT : (float)CURRENT_WINDOW_HEIGHT;

    glm::vec2 WIN_DIF = glm::vec2((float)CURRENT_WINDOW_WIDTH / (float)START_WINDOW_WIDTH, (float)CURRENT_WINDOW_HEIGHT / (float)START_WINDOW_HEIGHT);

    float Left  = -WIN_WIDTH  / 240;
    float Right =  WIN_WIDTH  / 240;
    float Down  = -WIN_HEIGHT / 240;
    float Up    =  WIN_HEIGHT / 240;
    Projection = glm::ortho(
        Left  / Zoom,
        Right / Zoom,
        Down  / Zoom,
        Up    / Zoom,
        -1000.0f, 1000.0f);
    CSS.setMat4("Projection", Projection);

    glBindVertexArray(VAO);

    glm::mat4 ResultPosition = glm::mat4(1.0f);

    if (OBJ.Type != RO_UI) {
        ResultPosition = glm::rotate(ResultPosition, -Camera->Rotation, glm::vec3(0, 0, 1));
    }

    ResultPosition = glm::translate(ResultPosition, glm::vec3(OBJ.PositionVisual * (OBJ.Type == RO_UI ? (OBJ.Resize ? ScreenScale : ScreenScale * WIN_DIF) : glm::vec2(1, 1)), (OBJ.Layer + (float)OBJ.GetID() / 100) / 100));
    if (OBJ.Type != RO_UI) {
        ResultPosition = glm::translate(ResultPosition, glm::vec3(Camera->Position.x, Camera->Position.y, 0));
    }

    ResultPosition = glm::rotate(ResultPosition, -OBJ.OrientationVisual, glm::vec3(0, 0, 1));
    ResultPosition = glm::scale(ResultPosition, glm::vec3(OBJ.SizeVisual, 1));

    CSS.setMat4("Position", ResultPosition);

    CSS.setBool("DebugRender", DebugRender);
    if (OBJ.Type == RO_Phys) {
        CSS.setBool("Sleeping", !b2Body_IsAwake(GetBody(OBJ.BodyID)));
    }
    else {
        CSS.setBool("Sleeping", false);
    }

    glDrawArrays(GL_QUADS, 0, square_l);
}

/* Рендерить линию */
void RenderLine(const GameObject& OBJ) {
    /* WIP */
    /* ==== Трансформация ==== */
    glm::mat4 Projection = glm::mat4(1.0f);
    float Zoom = 1 / (OBJ.Type == RO_UI ? 1 : Camera->Zoom);

    float WIN_WIDTH = OBJ.Resize ? (float)START_WINDOW_WIDTH : (float)CURRENT_WINDOW_WIDTH;
    float WIN_HEIGHT = OBJ.Resize ? (float)START_WINDOW_HEIGHT : (float)CURRENT_WINDOW_HEIGHT;

    float Left  = -WIN_WIDTH  / 240;
    float Right =  WIN_WIDTH  / 240;
    float Down  = -WIN_HEIGHT / 240;
    float Up    =  WIN_HEIGHT / 240;
    Projection = glm::ortho(
        Left  / Zoom,
        Right / Zoom,
        Down  / Zoom,
        Up    / Zoom,
        -1000.0f, 1000.0f);
    CSS.setMat4("Projection", Projection);

    glBindVertexArray(VAO);

    float Thickness = OBJ.SizeVisual.x;
    glm::vec2 StartPos = glm::vec2(OBJ.LinePositionVisual.x, OBJ.LinePositionVisual.y);
    glm::vec2 EndPos = glm::vec2(OBJ.LinePositionVisual.z, OBJ.LinePositionVisual.w);
    glm::vec2 CenterPos = StartPos / glm::vec2(2, 2) + EndPos / glm::vec2(2, 2);

    glm::mat4 ResultPosition = glm::mat4(1.0f);
    
    glm::vec2 Direction = EndPos - StartPos;
    float rad = atan2(Direction.y, Direction.x) - glm::half_pi<float>();;

    if (OBJ.Type != RO_UI) {
        ResultPosition = glm::rotate(ResultPosition, -Camera->Rotation, glm::vec3(0, 0, 1));
    }

    glm::vec2 Pos = CenterPos * (OBJ.Type == RO_UI ? ScreenScale : glm::vec2(1, 1));
    ResultPosition = glm::translate(ResultPosition, glm::vec3(Pos, (OBJ.Layer + (float)OBJ.GetID() / 10000) / 100 ));

    if (OBJ.Type != RO_UI) {
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
        if (!OBJ.Deleted && OBJ.Active && OBJ.Render) {
            int OBJ_Texture = OBJ.BaseTexture;
            if (OBJ_Texture != TEX) {
                TEX = OBJ_Texture;
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, TEX);
            }

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

/* Позицию экранную в мировую */
glm::vec2 ScreenPositionToWorld(glm::vec2 Pos, bool IgnoreCamera, bool Resize) {
    glm::vec2 Result = Pos;
    //800px => 20.0f / 3
    Result *= ScreenScale;
    if (!Resize) {
        Result *= glm::vec2((float)CURRENT_WINDOW_WIDTH / (float)START_WINDOW_WIDTH, (float)CURRENT_WINDOW_HEIGHT / (float)START_WINDOW_HEIGHT);
    }
    if (!IgnoreCamera) {
        Result *= Camera->Zoom;
        Result = glm::rotate(Result, Camera->Rotation);
        Result -= Camera->Position;
    }
    return Result;
}