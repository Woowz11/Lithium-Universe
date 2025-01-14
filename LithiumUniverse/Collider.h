#ifndef COLLIDER_H
#define COLLIDER_H

#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>
#include <vector>

#include "Console.h";

/* Тип коллайдера */
enum CLDR_Type {
	CLDR_None   = 0,
	CLDR_Point  = 1,
	CLDR_Line   = 2,
	CLDR_Custom = 3
};

class Collider {
private:
	const std::vector<glm::vec2> COLLIDER_SQUARE = {
		glm::vec2(-1,  1), glm::vec2(1,  1),
		glm::vec2(1, -1), glm::vec2(-1, -1)
	};
	const std::vector<glm::vec2> COLLIDER_CIRCLE = {
		glm::vec2( 0,  1), glm::vec2( 0.8f,  0.6f),
		glm::vec2( 1,  0), glm::vec2( 0.6f, -0.8f),
		glm::vec2( 0, -1), glm::vec2(-0.8f, -0.6f),
		glm::vec2(-1,  0), glm::vec2(-0.8f,  0.6f)
	};

	std::vector<glm::vec2> Points = COLLIDER_SQUARE;
public:
	CLDR_Type Type;

	Collider(CLDR_Type type){
		Type = type;
	}

	Collider& operator=(const Collider& other) {
		if (this == &other) {
			return *this;
		}

		Type = other.Type;
		Points = other.Points;

		return *this;
	}

	/* Сделать квадратным */
	void SetSquare() {
		SetPoints(COLLIDER_SQUARE);
	}

	/* Сделать круглым */
	void SetCircle() {
		SetPoints(COLLIDER_CIRCLE);
	}

	/* Установить свои точки коллайдера */
	void SetPoints(std::vector<glm::vec2> p) {
		if (Type != CLDR_Custom) {
			Warn("COLLIDER","There is no point in setting collider points because a different collider mode is set, not CLDR_Custom");
		}
		if (p.size() < 3) {
			Error("COLLIDER","There must be at least 3 points of collider!");
		}
		else {
			Points = p;
		}
	}

	/* Получить точки коллайдера */
	std::vector<glm::vec2> GetPoints() {
		if (Type != CLDR_Custom || Points.size() < 3) {
			Error("COLLIDER", "There is no point in getting collider points because a different collider mode is set, not CLDR_Custom");
			Error("COLLIDER", "Or there are less than 3 collider points!");
			return COLLIDER_SQUARE;
		}
		else {
			return Points;
		}
	}

	/* Получить точки коллайдера в мировых координатах*/
	std::vector<glm::vec2> GetPhysicalPoints(glm::vec2 Position, glm::vec2 Scale, float Orientation) {
		std::vector<glm::vec2> P = GetPoints();
		std::vector<glm::vec2> Result = {};
		for (glm::vec2 p : P) {
			glm::vec2 rp = p;
			rp /= glm::vec2(2, 2);
			rp *= Scale;
			rp = glm::rotate(rp, -glm::radians(Orientation));
			rp += Position;
			Result.push_back(rp);
		}
		return Result;
	}
};

#endif