#pragma once
#include <string>

void UpdateScene();
void Render();
void InstallRender(std::string GamePath_, uint32_t SWW, uint32_t SWH);
void ClearRender();
void MoveCamera(float vel_x, float vel_y);
void SetCameraPosition(float x, float y);
void MoveCameraZoom(float vel);
void SetCameraZoom(float z);
void UpdateDeltaTime(float DT);
void UpdateWindowSize(uint32_t W, uint32_t H);