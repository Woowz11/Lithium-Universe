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

	HitResult(GameObject* ho, glm::vec2 hp) {
		HitObject = ho;
		HitPosition = hp;
		Hit = true;
	}
	HitResult() {
		Hit = false;
	}
};

#endif