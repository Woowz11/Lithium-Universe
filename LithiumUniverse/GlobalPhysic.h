#pragma once
#include <vector>

#include "RenderedObject.h";

void InstallPhysic();
void UpdatePhysic(std::vector<RenderedObject>& Scene);
void UpdateDeltaTime_PHYSIC(float DT);