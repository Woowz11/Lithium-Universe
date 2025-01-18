#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>

#include "GameObjectActions.h";

enum Screen {
	MainMenu = 0, /* Главное меню */
	Game     = 1  /* Игра         */
};

/* Текущее окно */
Screen CurrentScreen = Game;

/* Окно в фокусе? */
bool GameInFocus = true;

/* Сделать объект интерфейсом (private) */
void MakeGameObjectUI__(int i) {
	GameObject& OBJ = GetGameObject(i, "MakeGameObjectUI__(" + std::to_string(i) + ");");
	OBJ.Type = RO_UI;
	SetGameObjectSelectable(i, false);
	OBJ.ThatUI = true;
	OBJ.Resize = true;
	OBJ.Layer = 10000;
}

/* Точка пересекает прямоугольник? */
bool PointOverBox(glm::vec2 Point, glm::vec2 Pos, glm::vec2 Size) {
	glm::vec2 p = Pos - Size * 0.5f;
	return (
		Point.x >= p.x &&
		Point.x <= p.x + Size.x &&
		Point.y >= p.y &&
		Point.y <= p.y + Size.y
	);
}

/* Создать интерфейс */
void CreateUI() {
	//int ui = CreateGameObject("ui", RO_UI);
}