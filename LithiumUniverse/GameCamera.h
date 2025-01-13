#ifndef GAME_CAMERA_H
#define GAME_CAMERA_H

#include <GLM/glm.hpp>

class GameCamera {
private:
	float dt = 0;
public:
	const float Speed = 2;
	const float ZoomSpeed = 0.5f;

	glm::vec2 Position = glm::vec2(0,0);
	float Zoom = 1;

	GameCamera() {}

	/* Установить DeltaTime для камеры */
	void SetDeltaTime(float dt_) {
		dt = dt_;
	}

	/* Двигать камеру */
	void MoveCamera(float vel_x, float vel_y) {
		Position = Position + glm::vec2(vel_x * Speed * Zoom * dt, vel_y * Speed * Zoom * dt);
	}

	/* Установить позицию камере */
	void SetCameraPosition(float x, float y) {
		Position = glm::vec2(x, y);
	}

	/* Двигать масштаб камеры */
	void MoveCameraZoom(float vel) {
		Zoom += (-vel * ZoomSpeed * dt) * Zoom;
	}

	/* Изменить масштаб камеры */
	void SetCameraZoom(float z) {
		Zoom = z;
	}
};

#endif