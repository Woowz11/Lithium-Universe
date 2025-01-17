#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>

#include "GlobalRender.h";
#include "GlobalPhysic.h";
#include "GameInstalls.h";
#include "GameCamera.h";
#include "GameData.h";

/* Скрипт отвечающий за управление игроком */

/* Объект который держит игрок мышкой */
int PickupedObject = -1;

/* Поднять объект */
void MousePickupGO(int i) {
	if (i != PickupedObject) {
		PickupedObject = i;
	}
}

/* Бросить объект */
void MouseDropGO() {
	PickupedObject = -1;
}

/* ==== Основа ==== */

/* Курсор двигается */
void MouseMove(glm::vec2 Pos, glm::vec2 Pos2) {
	
}

/* Кнопка на мыши нажата */
void MouseClick(int KEY, int ACTION) {
	if (KEY == GLFW_MOUSE_BUTTON_RIGHT && ACTION == GLFW_PRESS) {
		CreateTestObject(0);
	}

	if (KEY == GLFW_MOUSE_BUTTON_LEFT && ACTION == GLFW_PRESS) {
		if (ACTION == GLFW_PRESS) {
			MousePickupGO(GetMouseObject());
		}
		if (ACTION == GLFW_RELEASE) {
			MouseDropGO();
		}
	}
}

/* Управление клавиатурой */
bool SpacePressed = false;
void ControlsKeyboard(int KEY, int ACTION) {
	if (KEY == GLFW_KEY_SPACE && ACTION == GLFW_PRESS) {
		SpacePressed = !SpacePressed;
		SetSimulationSpeed(SpacePressed ? 0 : 1);
	}

	if (KEY == GLFW_KEY_Q && ACTION == GLFW_PRESS) {
		SpacePressed = false;
		SetSimulationSpeed(0.1f);
	}

	if (KEY == GLFW_KEY_1 && ACTION == GLFW_PRESS) {
		CreateTestObject(0);
	}

	if (KEY == GLFW_KEY_2 && ACTION == GLFW_PRESS) {
		CreateTestObject(1);
	}
}

/* Управление клавиатурой (Каждый тик) */
void ControlsKeyboardTick() {
	if (KeyPressed(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		ExitGame();
	}

	if (KeyPressed(GLFW_KEY_HOME) == GLFW_PRESS) {
		Camera->SetCameraRotation(0);
		Camera->SetCameraZoom(1);
		Camera->SetCameraPosition(0, 0);
	}

	bool W = KeyPressed(GLFW_KEY_W) == GLFW_PRESS;
	bool S = KeyPressed(GLFW_KEY_S) == GLFW_PRESS;
	bool D = KeyPressed(GLFW_KEY_D) == GLFW_PRESS;
	bool A = KeyPressed(GLFW_KEY_A) == GLFW_PRESS;

	bool SHIFT   = KeyPressed(GLFW_KEY_LEFT_SHIFT  ) == GLFW_PRESS;
	bool CONTROL = KeyPressed(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

	float speed = (SHIFT ? 3 : (CONTROL ? 0.3f : 1));

	if (W && !S) {
		Camera->MoveCamera(0, speed);
	}
	if (S && !W) {
		Camera->MoveCamera(0, -speed);
	}

	if (D && !A) {
		Camera->MoveCamera(speed, 0);
	}
	if (A && !D) {
		Camera->MoveCamera(-speed, 0);
	}

	bool PLUS  = KeyPressed(GLFW_KEY_EQUAL) == GLFW_PRESS;
	bool MINUS = KeyPressed(GLFW_KEY_MINUS) == GLFW_PRESS;

	if (PLUS && !MINUS) {
		Camera->MoveCameraZoom(speed);
	}
	if (MINUS && !PLUS) {
		Camera->MoveCameraZoom(-speed);
	}

	bool RIGHT = KeyPressed(GLFW_KEY_RIGHT) == GLFW_PRESS;
	bool LEFT  = KeyPressed(GLFW_KEY_LEFT ) == GLFW_PRESS;

	if (RIGHT && !LEFT) {
		Camera->MoveCameraRotation(speed);
	}
	if (LEFT && !RIGHT) {
		Camera->MoveCameraRotation(-speed);
	}
}

/* Управление */
void Controls() {
	ControlsKeyboardTick();
}