#pragma once
#include <box2d/box2d.h>
#include <GLM/glm.hpp>

#include <string>

#include "GameCamera.h";

/* Сцены */
enum Scenes {
	SCENE_NotSelected = -1, /* Ещё не выбрано */
	SCENE_MainMenu    =  0, /* Главное меню   */
	SCENE_Game        =  1  /* Игра           */
};


extern float Time;
extern float DeltaTime;
extern float GameDeltaTime;
extern float FPS;
extern float MS;
extern bool DeveloperVersion;

extern glm::vec2 MousePositionScreen;
extern glm::vec2 MousePosition;
extern glm::vec2 MousePositionNonResize;
extern glm::vec2 MouseWorldPosition;

extern uint32_t START_WINDOW_WIDTH;
extern uint32_t START_WINDOW_HEIGHT;
extern uint32_t CURRENT_WINDOW_WIDTH;
extern uint32_t CURRENT_WINDOW_HEIGHT;

extern std::string Version;
extern std::string GamePath;

extern GameCamera* Camera;
extern b2WorldId World;

extern Scenes CurrentScene;
void SetScene(Scenes Scen);

void ClearData();