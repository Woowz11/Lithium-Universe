#include "GameCamera.h";

/* Игровая камера */
GameCamera* Camera = new GameCamera();

/* Очистить данные */
void ClearData() {
	delete Camera;
}