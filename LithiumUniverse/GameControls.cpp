#include <GLFW/glfw3.h>

#include "GlobalRender.h";
#include "GameInstalls.h";

/* Скрипт отвечающий за управление игроком */

/* Управление клавиатурой */
void ControlsKeyboard() {
	if (KeyPressed(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		ExitGame();
	}

	if (KeyPressed(GLFW_KEY_HOME) == GLFW_PRESS) {
		SetCameraZoom(1);
		SetCameraPosition(0, 0);
	}

	bool W = KeyPressed(GLFW_KEY_W) == GLFW_PRESS;
	bool S = KeyPressed(GLFW_KEY_S) == GLFW_PRESS;
	bool D = KeyPressed(GLFW_KEY_D) == GLFW_PRESS;
	bool A = KeyPressed(GLFW_KEY_A) == GLFW_PRESS;

	bool SHIFT = KeyPressed(GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
	bool CONTROL = KeyPressed(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

	float speed = (SHIFT ? 3 : (CONTROL ? 0.3f : 1));

	if (W && !S) {
		MoveCamera(0, speed);
	}
	if (S && !W) {
		MoveCamera(0, -speed);
	}

	if (D && !A) {
		MoveCamera(speed, 0);
	}
	if (A && !D) {
		MoveCamera(-speed, 0);
	}

	bool PLUS  = KeyPressed(GLFW_KEY_EQUAL) == GLFW_PRESS;
	bool MINUS = KeyPressed(GLFW_KEY_MINUS) == GLFW_PRESS;

	if (PLUS && !MINUS) {
		MoveCameraZoom(speed);
	}
	if (MINUS && !PLUS) {
		MoveCameraZoom(-speed);
	}
}

/* Управление */
void Controls() {
	ControlsKeyboard();
}