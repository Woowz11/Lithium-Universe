#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>
#include <iostream>
#include "Console.h";

class GameCamera {
private:
	float dt = 0;
	float RealCameraRotation = 0;
public:
	glm::vec2 Position = glm::vec2(0,0);
	float Rotation = 0;
	float Zoom = 1;

	GameCamera() {}

	/* Установить DeltaTime для камеры */
	void SetDeltaTime(float dt_) {
		dt = dt_;
	}

	/* Двигать камеру */
	void MoveCamera(float vel_x, float vel_y) {
		Position = Position + glm::rotate(-glm::vec2(vel_x * 2 * Zoom * dt, vel_y * 2 * Zoom * dt), glm::radians(Rotation));
	}

	/* Установить позицию камере */
	void SetCameraPosition(float x, float y) {
		Position = glm::vec2(x, y);
	}

	/* Двигать масштаб камеры */
	void MoveCameraZoom(float vel) {
		Zoom += (-vel * 0.5f * dt) * Zoom;
	}

	/* Изменить масштаб камеры */
	void SetCameraZoom(float z) {
		Zoom = z;
	}

	/* Повернуть камеру */
	void MoveCameraRotation(float vel) {
		SetCameraRotation(RealCameraRotation - (vel * 50 * dt));
	}

	/* Установить поворот камеры */
	void SetCameraRotation(float deg) {
		RealCameraRotation = deg;
		Rotation = (floor(RealCameraRotation/15)*15);
	}
};

#endif