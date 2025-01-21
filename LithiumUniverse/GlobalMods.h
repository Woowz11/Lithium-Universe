#pragma once
#include <vector>
#include "GameMod.h";

extern std::vector<GameMod> Mods;

void CheckMods();
void StopMods();
void RunMods();