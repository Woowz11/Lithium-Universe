#pragma once
#include <string>

extern const int GameVersionMajor;
extern const int GameVersionMinor;
extern const int GameVersionPatch;

std::string GetGameVersionPatchWithLetters();
std::string GetGameVersion();