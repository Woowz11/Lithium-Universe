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
#include "GlobalResources.h";
#include "GlobalPhysic.h";
#include "GameCamera.h";
#include "GameObject.h";
#include "GameData.h";
#include "FontChar.h";
#include "Texture.h";
#include "Console.h";
#include "Shader.h";
#include "Font.h";

uint32_t START_WINDOW_WIDTH;
uint32_t START_WINDOW_HEIGHT;
uint32_t CURRENT_WINDOW_WIDTH;
uint32_t CURRENT_WINDOW_HEIGHT;
void UpdateWindowSize(uint32_t W, uint32_t H) {
    CURRENT_WINDOW_WIDTH  = W;
    CURRENT_WINDOW_HEIGHT = H;
}

LARGE_INTEGER AppTimeFrequency, AppTimeStart, AppTimeEnd;

/* Время шейдера */
float ShaderTime = 0;
float ShaderTimeOffset = 0;
void ReloadShaderTime() {
    ShaderTimeOffset = Time;
    ShaderTime = 0;
}

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
    -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,    0,
     1.0f, -1.0f, -1.0f,    1.0f, 0.0f,    1,
     1.0f,  1.0f, -1.0f,    1.0f, 1.0f,    2,
    -1.0f,  1.0f, -1.0f,    0.0f, 1.0f,    3
};
int square_l = 4; /* Кол-во строк в square */

/* Установить всё для рендера */
void InstallRender(uint32_t SWW, uint32_t SWH) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    START_WINDOW_WIDTH = SWW;
    START_WINDOW_HEIGHT = SWH;

    QueryPerformanceFrequency(&AppTimeFrequency);
    QueryPerformanceCounter(&AppTimeStart);
}

/* Установить всё для рендера после загрузки ресурсов */
void InstallRenderAfterResources() {

    /* ==== Вертиксы ====*/

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    /* ==== Физика ==== */

    InstallPhysic();
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
    glBindVertexArray(VAO);

    CSS.setInt("ID", OBJ.GetID());

    CSS.setVec2("Position", OBJ.PositionVisual);
    CSS.setFloat("Orientation", -OBJ.OrientationVisual);
    CSS.setVec2("Size", OBJ.SizeVisual);
    CSS.setFloat("Layer", OBJ.Layer);

    CSS.setBool("Static", OBJ.Static);
    CSS.setBool("Physical", OBJ.Type == RO_Phys);
    CSS.setBool("Interface", OBJ.Type == RO_UI);
    CSS.setBool("Resize", OBJ.Resize);
    CSS.setBool("DebugRender", DebugRender);
    if (OBJ.Type == RO_Phys) {
        CSS.setBool("Sleeping", OBJ.Static ? false : !b2Body_IsAwake(GetBody(OBJ.BodyID)));
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
    
    /*glm::mat4 Projection = glm::mat4(1.0f);
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

    glDrawArrays(GL_QUADS, 0, square_l);*/
}

/* Рендер текста */
void RenderText(const GameObject& OBJ) {
    std::u32string Text = OBJ.Text;
    Font F = Fonts[GetResourceAssetID(OBJ.FontRes)];

    CSS.setInt("ID", OBJ.GetID());
    CSS.setVec2("Position", OBJ.PositionVisual);
    CSS.setFloat("Orientation", -OBJ.OrientationVisual);
    CSS.setVec2("Size", OBJ.SizeVisual);
    CSS.setFloat("Layer", OBJ.Layer);

    CSS.setBool("Static", OBJ.Static);
    CSS.setBool("Physical", OBJ.Type == RO_Phys);
    CSS.setBool("Interface", OBJ.Type == RO_UI);
    CSS.setBool("Resize", OBJ.Resize);
    CSS.setBool("DebugRender", DebugRender);
    if (OBJ.Type == RO_Phys) {
        CSS.setBool("Sleeping", OBJ.Static ? false : !b2Body_IsAwake(GetBody(OBJ.BodyID)));
    }
    else {
        CSS.setBool("Sleeping", false);
    }

    CSS.setInt("TextLength", Text.size());

    int i = 0;
    int TotalW = 0;

    FontChar CinfoError = F.Chars[-1];
    CSS.setVec2("ErrorCharSize", glm::vec2(CinfoError.W, CinfoError.H));

    for (char32_t C : Text) {
        glBindVertexArray(VAO);

        uint32_t CharID = C;
        auto it = F.Chars.find(CharID);
        if (it == F.Chars.end()) {
            CharID = -1;
        }
        FontChar Cinfo = F.Chars[CharID];

        CSS.setInt("TextCharPosition", i);
        CSS.setInt("CharID", CharID);
        CSS.setVec2("CharPosition", glm::vec2(Cinfo.X, Cinfo.Y));
        CSS.setVec2("CharSize", glm::vec2(Cinfo.W, Cinfo.H));
        CSS.setInt("TextCharSize", TotalW);

        glDrawArrays(GL_QUADS, 0, square_l);
        i++;
        TotalW += Cinfo.W;
    }
}

/* Рендер картинки каждый кадр */
void Render(std::vector<GameObject>& Scene) {
	glClearColor(BackgroundColor.r, BackgroundColor.g, BackgroundColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Time = static_cast<double>(AppTimeEnd.QuadPart - AppTimeStart.QuadPart) / AppTimeFrequency.QuadPart;
    ShaderTime = Time - ShaderTimeOffset;
    float Rand = (static_cast<double>(rand()) / RAND_MAX);
    glm::vec2 ScreenSize = glm::vec2(CURRENT_WINDOW_WIDTH, CURRENT_WINDOW_HEIGHT);
    glm::vec2 ScreenStartSize = glm::vec2(START_WINDOW_WIDTH, START_WINDOW_HEIGHT);

    /* ==== Рендер объектов ==== */
    for (const GameObject& OBJ : Scene) {
        if (!OBJ.Deleted && OBJ.Active && OBJ.Render) {
            Texture OBJ_Texture = Texturies[GetResourceAssetID(OBJ.BaseTextureRes)];
            if (OBJ_Texture.ID != TEX) {
                TEX = OBJ_Texture.ID;
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, TEX);
            }

            Shader OBJ_Shader = Shaders[GetResourceAssetID(OBJ.BaseShaderRes)];
            CSS = OBJ_Shader;
            glUseProgram(CSS.ID);

            CSS.setVec2("TextureSize", glm::vec2(OBJ_Texture.Width, OBJ_Texture.Height));

            CSS.setVec2("ScreenStartSize", ScreenStartSize);
            CSS.setVec2("ScreenSize", ScreenSize);
            CSS.setVec2("MousePosition", MousePositionScreen);

            CSS.setVec2("CameraPosition", Camera->Position);
            CSS.setFloat("CameraOrientation", Camera->Orientation);
            CSS.setFloat("CameraZoom", Camera->Zoom);

            CSS.setFloat("Random" , Rand);
            CSS.setFloat("LocalRandom", (static_cast<double>(rand()) / RAND_MAX));

            QueryPerformanceCounter(&AppTimeEnd);
            CSS.setFloat("Time", ShaderTime);

            CSS.setVec4("Color", OBJ.Color);

            switch (OBJ.RenderType)
            {
            case RT_Line:
                RenderLine(OBJ);
                break;
            case RT_Text:
                RenderText(OBJ);
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
        Result = glm::rotate(Result, Camera->Orientation);
        Result -= Camera->Position;
    }
    return Result;
}