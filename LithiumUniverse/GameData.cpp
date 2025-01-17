#include <box2d/box2d.h>
#include "GameCamera.h";

/* Игровая камера */
GameCamera* Camera = new GameCamera();

/* Игровой мир для Box2D */
b2WorldId World = {0};

/* Очистить данные */
void ClearData() {
	delete Camera;
}