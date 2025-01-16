#include <box2cpp/box2cpp.h>
#include "GameCamera.h";

/* Игровая камера */
GameCamera* Camera = new GameCamera();

/* Игровой мир для Box2D */
b2::World* World = nullptr;

/* Очистить данные */
void ClearData() {
	delete Camera;
}