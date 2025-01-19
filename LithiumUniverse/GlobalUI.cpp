#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>

#include "GameObjectActions.h";
#include "GlobalPhysic.h";
#include "GlobalRender.h";

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
	glm::vec2 p = Pos - Size;
	return (
		Point.x >= p.x &&
		Point.x <= p.x + Size.x * 2 &&
		Point.y >= p.y &&
		Point.y <= p.y + Size.y * 2
	);
}

/* Точка пересекает прямоугольник? (Интерфейс) */
bool PointOverUIBox(glm::vec2 Point, glm::vec2 Pos, glm::vec2 Size) {
	return PointOverBox(ScreenPositionToWorld(Point, true), ScreenPositionToWorld(Pos, true), Size);
}
/* Обновление интерфейса */
int ui_test = -1;
void UpdateUI() {
	SetGameObjectPosition(ui_test, glm::vec2(sinf(Time), cosf(Time)));
	
	GameObject& ui = GetGameObject(ui_test, "chmona");
	SetGameObjectTexture(ui_test, PointOverUIBox(ui.Resize ? MousePosition : MousePositionNonResize, ui.PositionVisual, ui.SizeVisual) ? 1 : 2);
}

/* Создать интерфейс */
void CreateUI() {
	int ui = CreateGameObject("ui", RO_UI);
	//SetGameObjectResize(ui, false);
	//SetGameObjectStatic(ui, true);
	//SetGameObjectPosition(ui, glm::vec2(-1, 1));

	ui_test = ui;
}