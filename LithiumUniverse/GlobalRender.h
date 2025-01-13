#pragma once
#include <string>
#include <vector>
#include "RenderedObject.h";
#include "GameCamera.h";

void RenderAndPhysic();
void InstallRender(std::string GamePath_, uint32_t SWW, uint32_t SWH);
void ClearRender();
void UpdateDeltaTime(float DT);
void UpdateWindowSize(uint32_t W, uint32_t H);