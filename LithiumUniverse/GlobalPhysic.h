#pragma once
#include <GLM/glm.hpp>
#include <vector>

#include "RenderedObject.h";

void InstallPhysic();
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2);
void CreateScene(std::vector<RenderedObject>& Scene);
void UpdatePhysic(std::vector<RenderedObject>& Scene);
void UpdateDeltaTime_PHYSIC(float DT, float Time_);