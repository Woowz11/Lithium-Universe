#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>
#include <iostream>

#include "Console.h";

class GameCamera {
private:
	float RealCameraRotation = 0;
public:
	glm::vec2 Position = glm::vec2(0,0);
	float Rotation = 0;
	float Zoom = 1;

	GameCamera() {}

	/* Установить позицию камере */
	void SetCameraPosition(float x, float y) {
		Position = glm::vec2(x, y);
	}

	/* Двигать камеру */
	void MoveCamera(float vel_x, float vel_y, float dt) {
		Position = Position + glm::rotate(-glm::vec2(vel_x * 2 * Zoom * dt, vel_y * 2 * Zoom * dt), Rotation);
	}

	/* Изменить масштаб камеры */
	void SetCameraZoom(float z) {
		Zoom = z;
	}

	/* Двигать масштаб камеры */
	void MoveCameraZoom(float vel, float dt) {
		Zoom += (-vel * 0.5f * dt) * Zoom;
	}

	/* Установить поворот камеры */
	void SetCameraRotation(float deg) {
		RealCameraRotation = deg;
		Rotation = glm::radians(floor(glm::degrees(RealCameraRotation) / 15) * 15);
	}

	/* Повернуть камеру */
	void MoveCameraRotation(float vel, float dt) {
		SetCameraRotation(RealCameraRotation - (vel * dt));
	}
};

#endif