#pragma once
#include <vector>
#include <string>

#include "GameResource.h";

extern std::vector<GameResource> Resources;

int GetResourceAssetID(const int ID, const int IfNotFound);
int GetResourceAssetID(const int ID);
int GetResourceID(const int ID, const int IfNotFound);
int GetResourceID(const int ID);

std::string FullPathToComplex(const std::string FullPath);
std::string ComplexToFullPath(const std::string NotFullPath);
std::string GetBaseFromPath(const std::string Path);
GameResource GetResourceDebug(const std::string FullPath);
GameResource GetResource(const std::string Base, const std::string Path);
void UpdateResources();