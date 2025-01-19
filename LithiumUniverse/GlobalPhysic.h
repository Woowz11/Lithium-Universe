#pragma once
#include <GLM/glm.hpp>
#include <vector>

#include "GameObject.h";

extern int MouseObjectConnector;

void InstallPhysic(bool DV);
void ClearPhysic();
void CreateScene();
std::vector<GameObject>& UpdatePhysic();
void SetSimulationSpeed(float sp);
float GetSimulationSpeed();
void UpdateDeltaTimePhysic(float dt_);
void CreateTestObject(int type);
int GetMouseObject();