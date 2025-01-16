#pragma once
#include <GLM/glm.hpp>
#include <vector>

#include "GameObject.h";

void InstallPhysic();
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2);
void CreateScene();
std::vector<GameObject>& UpdatePhysic();
void SetSimulationSpeed(float sp);
float GetSimulationSpeed();
void UpdateDeltaTimePhysic(float dt_);
void CreateTestObject(int type);