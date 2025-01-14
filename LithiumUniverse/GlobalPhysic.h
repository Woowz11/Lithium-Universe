#pragma once
#include <GLM/glm.hpp>
#include <vector>

#include "GameObject.h";

void InstallPhysic();
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2);
void CreateScene(std::vector<GameObject>& Scene);
void UpdatePhysic(std::vector<GameObject>& Scene);
void UpdateDeltaTime_PHYSIC(float DT, float Time_);