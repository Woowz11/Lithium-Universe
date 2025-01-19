#pragma once
#include <vector>
#include <string>

#include "GameResource.h";

extern std::vector<GameResource> Resources;

GameResource GetResourceDebug(std::string FullPath);
void UpdateResources();