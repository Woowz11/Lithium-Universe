#pragma once
#include <vector>

#include "RenderedObject.h";

void InstallPhysic();
void CreateScene(std::vector<RenderedObject>& Scene);
void UpdatePhysic(std::vector<RenderedObject>& Scene);
void UpdateDeltaTime_PHYSIC(float DT);