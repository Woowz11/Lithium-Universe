#pragma once
#include <box2d/box2d.h>
#include "GameCamera.h";

extern float Time;
extern float DeltaTime;
extern float GameDeltaTime;
extern bool DeveloperVersion;

extern GameCamera* Camera;
extern b2WorldId World;

void ClearData();