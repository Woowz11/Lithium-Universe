#pragma once
#include <GLM/glm.hpp>
#include <string>
#include <vector>

#include "GameCamera.h";
#include "GameObject.h";

void Render(std::vector<GameObject>& Scene);
void InstallRender(std::string GamePath_, uint32_t SWW, uint32_t SWH, bool DV);
void ClearRender();
void UpdateDeltaTime(float DT);
void UpdateWindowSize(uint32_t W, uint32_t H);
glm::vec2 ScreenPositionToWorld(glm::vec2 Pos, bool IgnoreCamera, bool Resize);