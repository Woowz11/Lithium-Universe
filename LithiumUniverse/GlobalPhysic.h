#pragma once
#include <GLM/glm.hpp>
#include <vector>

#include "GameObject.h";

extern int MouseObjectConnector;
extern int MouseObject;

void InstallPhysic();
void ClearPhysic();
void CreateScene();
std::vector<GameObject>& UpdatePhysic();
void SetSimulationSpeed(float sp);
float GetSimulationSpeed();
void UpdateDeltaTimePhysic(float dt_);
void CreateTestObject(int type);
void RemoveLastTestObject();
void RemoveAllTestObject();