#pragma once
#include <GLM/glm.hpp>
#include <vector>

#include "GameObject.h";

void SetPhysicTime(float npt);
void InstallPhysic();
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2);
void CreateScene();
std::vector<GameObject>& UpdatePhysic();
void UpdateDeltaTime_PHYSIC(float DT, float Time_);