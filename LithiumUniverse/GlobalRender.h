#pragma once
#include <string>

void Render();
void InstallRender(std::string GamePath_, uint32_t SWW, uint32_t SWH);
void ClearRender();
void MoveCamera(float vel_x, float vel_y);
void SetCameraPosition(float x, float y);
void MoveCameraZoom(float vel);
void SetCameraZoom(float z);