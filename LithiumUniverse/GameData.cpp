#include <box2d/box2d.h>
#include <GLM/glm.hpp>

#include <string>

#include "GameObjectActions.h";
#include "GlobalMods.h";
#include "GameCamera.h";
#include "GameData.h";
#include "Console.h";

#include "GlobalPhysic.h";
#include "GlobalUI.h";

/* Версия игры */
std::string Version = "";

/* Позиция папки с игрой */
std::string GamePath = "";

/* Прошедшее время с запуска игры */
float Time = 0;

/* Число сглаживающее время с фпс */
float DeltaTime = 0;

/* Число сглаживающее время с фпс и на которое влияет игровое время */
float GameDeltaTime = 0;

/* Игра скомпилирована в Dev режиме? */
bool DeveloperVersion = false;

/* Игровая камера */
GameCamera* Camera = new GameCamera();

/* Позиция мыши в экранных координатах (в пикселях) */
glm::vec2 MousePositionScreen = glm::vec2(0, 0);

/* Позиция мыши в экранных координатах */
glm::vec2 MousePosition = glm::vec2(0, 0);

/* Позиция мыши в экранных координатах (размер экрана влияет) */
glm::vec2 MousePositionNonResize = glm::vec2(0, 0);

/* Позиция мыши в мировых координатах */
glm::vec2 MouseWorldPosition = glm::vec2(0, 0);

/* Игровой мир для Box2D */
b2WorldId World = b2_nullWorldId;

/* Текущая сцена */
Scenes CurrentScene = SCENE_NotSelected;

/* Установить сцену */
void SetScene(Scenes Scen) {
	if (Scen != CurrentScene) {
		Print("SCENE", "Scene changed to (" + std::to_string(Scen) + ")!");
		CurrentScene = Scen;

		StopMods();

		int i = 0;
		for (GameObject& OBJ : Scene) {
			DeleteGameObject(i);

			i++;
		}
		Scene = {};
		Buttons = {};

		RunMods();

		CreateUI(Scen);
		CreateScene(Scen);
	}
}

/* Очистить данные */
void ClearData() {
	delete Camera;
}