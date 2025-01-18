#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>

#include "GameObjectActions.h";
#include "GlobalPhysic.h";

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
	OBJ.Resize = true;
	SetGameObjectLayer(i, 1000);
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

/* Обновление интерфейса */
int ui_test = -1;
void UpdateUI() {
	SetGameObjectPosition(ui_test, glm::vec2(sinf(Time), cosf(Time)));
}

/* Создать интерфейс */
void CreateUI() {
	int ui = CreateGameObject("ui", RO_UI);
	//SetGameObjectPosition(ui, glm::vec2(-1, 1));

	ui_test = ui;
}