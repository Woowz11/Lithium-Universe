#pragma once
#include <string>

#include "GameMod.h";

void RunScript(const std::string& ScriptPath);
void LoadLua(GameMod Mod);
void UnloadLua();
void InstallLua();