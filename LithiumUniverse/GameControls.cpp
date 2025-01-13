#include <GLFW/glfw3.h>

#include "GlobalRender.h";
#include "GameInstalls.h";
#include "GameCamera.h";
#include "GameData.h";
/* Скрипт отвечающий за управление игроком */

/* Управление клавиатурой */
void ControlsKeyboard() {
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
	ControlsKeyboard();
}