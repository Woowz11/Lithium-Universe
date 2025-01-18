#pragma once
#include <GLM/glm.hpp>
#include <string>
#include <vector>
#include "GameCamera.h";

void RenderAndPhysic();
void InstallRender(std::string GamePath_, uint32_t SWW, uint32_t SWH, bool DV);
void ClearRender();
void UpdateDeltaTime(float DT);
void UpdateWindowSize(uint32_t W, uint32_t H);
void MouseRenderMove(glm::vec2 Pos, glm::vec2 Pos2);
glm::vec2 ScreenPositionToWorld(glm::vec2 Pos);