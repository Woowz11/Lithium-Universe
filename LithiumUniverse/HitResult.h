#ifndef HIT_RESULT_H
#define HIT_RESULT_H

#include <GLM/glm.hpp>
#include "GameObject.h";

/* Скрипт на информации о поподании */

class HitResult {
public:
	bool Hit;
	glm::vec2 HitPosition;
	GameObject* HitObject;
	float Depth;

	HitResult(GameObject* ho, glm::vec2 hp, float d) {
		HitObject = ho;
		HitPosition = hp;
		Depth = d;
		Hit = true;
	}
	HitResult(GameObject* ho, glm::vec2 hp) {
		HitObject = ho;
		HitPosition = hp;
		Depth = 0;
		Hit = true;
	}
	HitResult() {
		HitObject = nullptr;
		HitPosition = glm::vec2(0,0);
		Depth = 0;
		Hit = false;
	}
};

#endif