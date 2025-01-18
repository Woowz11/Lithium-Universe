#pragma once
#include <GLM/glm.hpp>
#include <vector>

#include "GameObject.h";

extern int MouseObjectConnector;
extern glm::vec2 MouseWorldPosition;
extern bool DeveloperVersion;

void InstallPhysic(bool DV);
void ClearPhysic();
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2);
void CreateScene();
std::vector<GameObject>& UpdatePhysic();
void SetSimulationSpeed(float sp);
float GetSimulationSpeed();
void UpdateDeltaTimePhysic(float dt_);
void CreateTestObject(int type);
int GetMouseObject();