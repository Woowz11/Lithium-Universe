#pragma once
#include <GLM/glm.hpp>
#include <string>
#include <vector>

#include "GameCamera.h";
#include "GameObject.h";

void Render(const std::vector<GameObject>& Scene);
void InstallRender();
void InstallRenderAfterResources();
void ClearRender();
bool PointInsideCamera(const glm::vec2 Pos, const float PointSize);
glm::vec2 ScreenPositionToWorld(const glm::vec2 Pos, const bool IgnoreCamera, const bool Resize);
void ReloadShaderTime();