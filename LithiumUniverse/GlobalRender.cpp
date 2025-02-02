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

/* Элементы для высчита времени приложения */
LARGE_INTEGER AppTimeFrequency, AppTimeStart, AppTimeEnd;

/* Время шейдера */
float ShaderTime = 0;
float ShaderTimeOffset = 0;
void ReloadShaderTime() {
    ShaderTimeOffset = Time;
    ShaderTime = 0;
}

/* Константа на размер окна */
const glm::vec2 ScreenScale = glm::vec2(10.0f / 3, 2.5f);

/* Отладочный рендер */
const bool DebugRender = false;

/* ==== Глобальное ==== */

/* Цвет заднего фона */
glm::vec3 BackgroundColor = glm::vec3(0.1f, 0.1f, 0.1f);

/* ==== Доп. функции ==== */

/* Позицию экранную в мировую */
glm::vec2 ScreenPositionToWorld(const glm::vec2 Pos, const bool IgnoreCamera, const bool Resize) {
    glm::vec2 Result = Pos;
    //800px => 20.0f / 3
    Result *= ScreenScale;
    if (!Resize) {
        Result *= glm::vec2((float)CURRENT_WINDOW_WIDTH / (float)START_WINDOW_WIDTH, (float)CURRENT_WINDOW_HEIGHT / (float)START_WINDOW_HEIGHT);
    }
    if (!IgnoreCamera) {
        Result *= Camera->Zoom;
        Result = glm::rotate(Result, -Camera->Orientation);
        Result -= -Camera->Position;
    }
    return Result;
}

/* Проверить, точка находится в пределах камеры? */
bool PointInsideCamera(const glm::vec2 Pos, float PointSize) {
    float CameraRotateFractor = fabs(fmod(-Camera->Orientation, 6.28319f))/6.28319f;
    CameraRotateFractor = -pow((CameraRotateFractor * 2 - 1),2) + 1;

    PointSize += PointSize * (CameraRotateFractor * 15);

    glm::vec2 TR = ScreenPositionToWorld(glm::vec2( 1,  1), false, false);
    glm::vec2 BL = ScreenPositionToWorld(glm::vec2(-1, -1), false, false);
    return (Pos.x + PointSize > BL.x && Pos.x - PointSize < TR.x &&
        Pos.y + PointSize > BL.y && Pos.y - PointSize < TR.y);
}

/* ==== Вертиксы ==== */

unsigned int VBO, VAO;
unsigned int TextVBO, TextVAO;
unsigned int PPVBO, PPVAO;
GLuint FBO;
GLuint RBO;
GLuint FBOT;

/* Вертиксы квадрата (вертиксы, развёртка, номер верт.) */
float Square[] = {
    -1.0f, -1.0f,    0.0f, 0.0f,    0,
     1.0f, -1.0f,    1.0f, 0.0f,    1,
     1.0f,  1.0f,    1.0f, 1.0f,    2,
    -1.0f,  1.0f,    0.0f, 1.0f,    3
};
int Square_l = 4; /* Кол-во строк в square */

/* Проверка на GL ошибку */
void CheckGLError(const std::string Script) {
#ifdef NDEBUG
#else
    GLenum E = glGetError();
    if (E != GLFW_NO_ERROR) {
        Error("GL CHECK", "Error ID (" + std::to_string(E) + ") " + Script);
    }
#endif
}

/* Установить всё для рендера */
void InstallRender() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QueryPerformanceFrequency(&AppTimeFrequency);
    QueryPerformanceCounter(&AppTimeStart);
    CheckGLError("InstallRender();");
}

void RenderWindowSizeChanged() {
    glBindTexture(GL_TEXTURE_2D, FBOT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, CURRENT_WINDOW_WIDTH, CURRENT_WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, CURRENT_WINDOW_WIDTH, CURRENT_WINDOW_HEIGHT);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Error("POSTP", "Framebuffer resize failed!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CheckGLError("RenderWindowSizeChanged();");
}

void CreateBuffers_Default() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Square), Square, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, 5 * sizeof(float), (void*)(4 * sizeof(float)));

    CheckGLError("CreateBuffers_Default();");
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

struct TextVertex {
    glm::vec2 Vertex;
    glm::vec2 UV;
    int Char;
    int PositionInString;
};

void CreateBuffers_Text() {
    glGenVertexArrays(1, &TextVAO);
    glGenBuffers(1, &TextVBO);

    glBindVertexArray(TextVAO);
    glBindBuffer(GL_ARRAY_BUFFER, TextVBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, Vertex));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (void*)offsetof(TextVertex, UV));

    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 1, GL_INT, sizeof(TextVertex), (void*)offsetof(TextVertex, Char));

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_INT, sizeof(TextVertex), (void*)offsetof(TextVertex, PositionInString));

    CheckGLError("CreateBuffers_Text();");
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CreateBuffers_PostProcessing() {
    glGenVertexArrays(1, &PPVAO);
    glGenBuffers(1, &PPVBO);

    glBindVertexArray(PPVAO);
    glBindBuffer(GL_ARRAY_BUFFER, PPVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Square), Square, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, 5 * sizeof(float), (void*)(4 * sizeof(float)));

    CheckGLError("CreateBuffers_PostProcessing();");
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &FBOT);
    glBindTexture(GL_TEXTURE_2D, FBOT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, START_WINDOW_WIDTH, START_WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBOT, 0);

    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, START_WINDOW_WIDTH, START_WINDOW_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Error("POSTP","Framebuffer was not created!");
    }

    CheckGLError("CreateBuffers_PostProcessing(); #Create Texture#");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


/* Установить всё для рендера после загрузки ресурсов */
void InstallRenderAfterResources() {
    CreateBuffers_Default();
    CreateBuffers_Text();
    CreateBuffers_PostProcessing();

    InstallPhysic();
}

/* Удалить всё что осталось после рендера */
void ClearRender() {
    ClearPhysic();
    
    glDeleteFramebuffers(1, &FBO);
    glDeleteRenderbuffers(1, &RBO);

    glDeleteVertexArrays(1, &PPVAO);
    glDeleteBuffers(1, &PPVBO);

    glDeleteVertexArrays(1, &TextVAO);
    glDeleteBuffers(1, &TextVBO);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    CheckGLError("ClearRender();");
}

/* ==== Рендер объектов ==== */

Shader* CSS = nullptr;
Texture* T = nullptr;

/* Рендер квадрата */
void RenderSquare(const GameObject& OBJ) {
    glm::vec2 Position = OBJ.PositionVisual;
    glm::vec2 Size = OBJ.SizeVisual;

    /*float TotalScale = fmax(Size.x + 0.5f, Size.y + 0.5f); //Говно обрезка в жопу её
    if (!PointInsideCamera(Position, TotalScale)) {
        return;
    }*/

    CSS->setInt(CSS->UNIFORM_ID, OBJ.GetID());

    CSS->setVec2(CSS->UNIFORM_POSITION, Position);
    CSS->setFloat(CSS->UNIFORM_ORIENTATION, -OBJ.OrientationVisual);
    CSS->setVec2(CSS->UNIFORM_SIZE, Size);
    CSS->setFloat(CSS->UNIFORM_LAYER, OBJ.Layer);

    CSS->setBool(CSS->UNIFORM_STATIC, OBJ.Static);
    CSS->setBool(CSS->UNIFORM_PHYSICAL, OBJ.Type == RO_Phys);
    CSS->setBool(CSS->UNIFORM_INTERFACE, OBJ.Type == RO_UI);
    CSS->setBool(CSS->UNIFORM_RESIZE, OBJ.Resize);
    CSS->setVec4(CSS->UNIFORM_COLOR, OBJ.Color);

    /*if (OBJ.Type == RO_Phys) {
        CSS.setBool("Sleeping", OBJ.Static ? false : !b2Body_IsAwake(GetBody(OBJ.BodyID)));
    }
    else {
        CSS.setBool("Sleeping", false);
    }*/

    glDrawArrays(GL_QUADS, 0, Square_l);
}

/* Рендерить линию */
void RenderLine(const GameObject& OBJ) {
    /* СМ. код на гитхабе */
}

/* Рендер текста */
void RenderText(const GameObject& OBJ) {
    glBindBuffer(GL_ARRAY_BUFFER, TextVBO);

    std::string Text = OBJ.Text;
    Font F = Fonts[OBJ.FontID];

    CSS->setInt(CSS->UNIFORM_ID, OBJ.GetID());
    CSS->setVec2(CSS->UNIFORM_POSITION, OBJ.PositionVisual);
    CSS->setBool(CSS->UNIFORM_INTERFACE, OBJ.Type == RO_UI);
    CSS->setBool(CSS->UNIFORM_RESIZE, OBJ.Resize);
    CSS->setFloat(CSS->UNIFORM_ORIENTATION, -OBJ.OrientationVisual);
    CSS->setVec2(CSS->UNIFORM_SIZE, OBJ.SizeVisual);
    CSS->setFloat(CSS->UNIFORM_LAYER, OBJ.Layer);
    CSS->setVec4(CSS->UNIFORM_COLOR, OBJ.Color);

    CSS->setBool(CSS->UNIFORM_STATIC, OBJ.Static);
    CSS->setBool(CSS->UNIFORM_PHYSICAL, OBJ.Type == RO_Phys);

    std::vector<TextVertex> TextVertices;
    float Indent = 0;
    float Spacing = (1.0f/8);
    for (size_t i = 0; i < Text.size(); i++) {
        int CharID = Text[i];
        auto it = F.Chars.find(CharID);
        if (it == F.Chars.end()) {
            CharID = -1;
        }
        FontChar& Cinfo = F.Chars[CharID];
        int MaxX = (F.MaxX + 1);
        int MaxY = (F.MaxY + 1);

        double W = Cinfo.W/MaxX;
        double X = (double)Cinfo.X/MaxX;
        double Y = (double)Cinfo.Y/MaxY;

        glm::vec2 pos[4] = {
            {Indent          , 0.0f},
            {Indent + Cinfo.W, 0.0f},
            {Indent + Cinfo.W, 1.0f},
            {Indent          , 1.0f}
        };

        glm::vec2 uv[4] = {
            {X,     Y + (1.0/MaxY)},
            {X + W, Y + (1.0/MaxY)},
            {X + W, Y             },
            {X,     Y             }
        };

        for (int j = 0; j < 4; j++) {
            TextVertices.push_back(TextVertex(pos[j], uv[j], CharID, i));
        }

        Indent += Cinfo.W + Spacing;
    }

    glBindBuffer(GL_ARRAY_BUFFER, TextVBO);
    glBufferData(GL_ARRAY_BUFFER, TextVertices.size() * sizeof(TextVertex), TextVertices.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(TextVAO);
    glDrawArrays(GL_QUADS, 0, TextVertices.size());
    glBindVertexArray(0);
}

/* Пост процессинг */
int TestPostProcessing = -1;
void PostProcessing() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (TestPostProcessing == -1) {
        TestPostProcessing = GetResource("Base", "Shaders/PostProcessing.lu_shader").AssetID;
    }
    Shader PP_Shader = Shaders[TestPostProcessing];
    glUseProgram(PP_Shader.ID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, FBOT);

    glBindVertexArray(PPVAO);
    glDrawArrays(GL_QUADS, 0, Square_l);
}

/* Рендер картинки каждый кадр */
void Render(const std::vector<GameObject>& Scene) {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(BackgroundColor.r, BackgroundColor.g, BackgroundColor.b, 1);

    Time = static_cast<double>(AppTimeEnd.QuadPart - AppTimeStart.QuadPart) / AppTimeFrequency.QuadPart;
    ShaderTime = Time - ShaderTimeOffset;

    float Rand = (static_cast<double>(rand()) / RAND_MAX);

    glm::vec2 ScreenSize = glm::vec2(CURRENT_WINDOW_WIDTH, CURRENT_WINDOW_HEIGHT);
    glm::vec2 ScreenStartSize = glm::vec2(START_WINDOW_WIDTH, START_WINDOW_HEIGHT);

    /* ==== Рендер объектов ==== */

    GLuint lastTexID = 0;
    GLuint lastShaderID = 0;

    RenderTypeEnum CurrentRenderType = RT_NotSelected;

    std::vector<GameObject> UI = {};

    for (const GameObject& OBJ : Scene) {
        if (!OBJ.Deleted && OBJ.Active && OBJ.Render) {
            if (OBJ.Type == RO_UI) {
                UI.push_back(OBJ);
            }
            else {
                Texture OBJ_Texture = Texturies[OBJ.BaseTextureID];
                if (OBJ_Texture.ID != lastTexID) {
                    lastTexID = OBJ_Texture.ID;
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, lastTexID);
                    T = &OBJ_Texture;
                }

                Shader OBJ_Shader = Shaders[OBJ.BaseShaderID];
                if (OBJ_Shader.ID != lastShaderID) {
                    lastShaderID = OBJ_Shader.ID;
                    glUseProgram(lastShaderID);
                    CSS = &OBJ_Shader;

                    CSS->setVec2(CSS->UNIFORM_TEXTURESIZE, glm::vec2(OBJ_Texture.Width, OBJ_Texture.Height));
                    CSS->setVec2(CSS->UNIFORM_SCREENSTARTSIZE, ScreenStartSize);
                    CSS->setVec2(CSS->UNIFORM_SCREENSIZE, ScreenSize);
                    CSS->setVec2(CSS->UNIFORM_MOUSEPOSITION, MousePositionScreen);
                    CSS->setVec2(CSS->UNIFORM_CAMERAPOSITION, -Camera->Position);
                    CSS->setFloat(CSS->UNIFORM_CAMERAORIENTATION, Camera->Orientation);
                    CSS->setFloat(CSS->UNIFORM_CAMERAZOOM, Camera->Zoom);
                    CSS->setFloat(CSS->UNIFORM_RANDOM, Rand);
                    CSS->setFloat(CSS->UNIFORM_LOCALRANDOM, (static_cast<double>(rand()) / RAND_MAX));
                    CSS->setFloat(CSS->UNIFORM_TIME, ShaderTime);
                    CSS->setBool(CSS->UNIFORM_DEBUGRENDER, DebugRender);
                }

                if (OBJ.RenderType != CurrentRenderType) {
                    CurrentRenderType = OBJ.RenderType;
                    switch (CurrentRenderType)
                    {
                    default:
                        glBindBuffer(GL_ARRAY_BUFFER, VBO);
                        glBindVertexArray(VAO);
                        break;
                    }
                }

                switch (CurrentRenderType) {
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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    /* ==== Постпроцессинг ==== */

    PostProcessing();

    /* ==== Рендер интерфейса ==== */

    lastTexID = 0;
    lastShaderID = 0;

    CurrentRenderType = RT_NotSelected;

    for (const GameObject& OBJ : UI) {
        Texture OBJ_Texture = Texturies[OBJ.BaseTextureID];
        if (OBJ_Texture.ID != lastTexID) {
            lastTexID = OBJ_Texture.ID;
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, lastTexID);
            T = &OBJ_Texture;
        }

        Shader OBJ_Shader = Shaders[OBJ.BaseShaderID];
        if (OBJ_Shader.ID != lastShaderID) {
            lastShaderID = OBJ_Shader.ID;
            glUseProgram(lastShaderID);
            CSS = &OBJ_Shader;

            CSS->setVec2(CSS->UNIFORM_TEXTURESIZE, glm::vec2(OBJ_Texture.Width, OBJ_Texture.Height));
            CSS->setVec2(CSS->UNIFORM_SCREENSTARTSIZE, ScreenStartSize);
            CSS->setVec2(CSS->UNIFORM_SCREENSIZE, ScreenSize);
            CSS->setVec2(CSS->UNIFORM_MOUSEPOSITION, MousePositionScreen);
            CSS->setVec2(CSS->UNIFORM_CAMERAPOSITION, -Camera->Position);
            CSS->setFloat(CSS->UNIFORM_CAMERAORIENTATION, Camera->Orientation);
            CSS->setFloat(CSS->UNIFORM_CAMERAZOOM, Camera->Zoom);
            CSS->setFloat(CSS->UNIFORM_RANDOM, Rand);
            CSS->setFloat(CSS->UNIFORM_LOCALRANDOM, (static_cast<double>(rand()) / RAND_MAX));
            CSS->setFloat(CSS->UNIFORM_TIME, ShaderTime);
            CSS->setBool(CSS->UNIFORM_DEBUGRENDER, DebugRender);
        }

        if (OBJ.RenderType != CurrentRenderType) {
            CurrentRenderType = OBJ.RenderType;
            switch (CurrentRenderType)
            {
            default:
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBindVertexArray(VAO);
                break;
            }
        }

        switch (CurrentRenderType) {
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

    CheckGLError("Render();");
}