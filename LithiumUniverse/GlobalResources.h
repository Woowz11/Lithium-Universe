#pragma once
#include <vector>
#include <string>

#include "GameResource.h";

extern std::vector<GameResource> Resources;

int GetResourceAssetID(int ID, int IfNotFound);
int GetResourceAssetID(int ID);
int GetResourceID(int ID, int IfNotFound);
int GetResourceID(int ID);

std::string ComplexToFullPath(std::string NotFullPath);
std::string GetBaseFromPath(std::string Path);
GameResource GetResourceDebug(std::string FullPath);
GameResource GetResource(std::string Base, std::string Path);
void UpdateResources();