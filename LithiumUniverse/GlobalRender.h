#pragma once

void Render();
void InstallRender(uint32_t SWW, uint32_t SWH);
void ClearRender();
void MoveCamera(float vel_x, float vel_y);
void SetCameraPosition(float x, float y);