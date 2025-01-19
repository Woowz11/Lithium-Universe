#pragma once
#include <box2d/box2d.h>
#include <GLM/glm.hpp>

#include "GameCamera.h";

extern float Time;
extern float DeltaTime;
extern float GameDeltaTime;
extern bool DeveloperVersion;

extern glm::vec2 MousePosition;
extern glm::vec2 MousePositionNonResize;
extern glm::vec2 MouseWorldPosition;

extern GameCamera* Camera;
extern b2WorldId World;

void ClearData();