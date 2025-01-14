#pragma once
#include <GLM/glm.hpp>
#include <vector>

#include "Console.h";

/* Тип коллайдера */
enum CLDR_Type {
	CLDR_None   = 0,
	CLDR_Point  = 1,
	CLDR_Line   = 2,
	CLDR_Square = 3,
	CLDR_Circle = 4,
	CLDR_Custom = -1
};

class Collider {
private:
	std::vector<glm::vec2> Points = {};
public:
	CLDR_Type Type;

	Collider(CLDR_Type type){
		Type = type;
	}

	void SetPoints(std::vector<glm::vec2> p) {
		if (Type != CLDR_Custom) {
			Warn("COLLIDER","There is no point in setting collider points because a different collider mode is set, not CLDR_Custom");
		}
		if (p.size() < 3) {
			Error("COLLIDER","There must be at least 3 points of collider!");
			Points = {
				glm::vec2(-1,  1), glm::vec2( 1,  1),
				glm::vec2( 1, -1), glm::vec2(-1, -1)
			};
		}
		else {
			Points = p;
		}
	}

	std::vector<glm::vec2> GetPoints() {
		if (Type != CLDR_Custom || Points.size() < 3) {
			Error("COLLIDER", "There is no point in getting collider points because a different collider mode is set, not CLDR_Custom");
			Error("COLLIDER", "Or there are less than 3 collider points!");
			return {
				glm::vec2(-1,  1), glm::vec2( 1,  1),
				glm::vec2( 1, -1), glm::vec2(-1, -1)
			};
		}
		else {
			return Points;
		}
	}

	std::vector<glm::vec2> GetPhysicalPoints(glm::vec2 Position ,glm::vec2 Scale) {
		std::vector<glm::vec2> P = GetPoints();
		std::vector<glm::vec2> Result = {};
		for (glm::vec2 p : P) {
			glm::vec2 rp = p;
			rp /= glm::vec2(2, 2);
			rp *= Scale;
			rp += Position;
			Result.push_back(rp);
		}
		return Result;
	}
};