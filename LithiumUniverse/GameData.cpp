#include <box2d/box2d.h>
#include "GameCamera.h";

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

/* Игровой мир для Box2D */
b2WorldId World = b2_nullWorldId;

/* Очистить данные */
void ClearData() {
	delete Camera;
}