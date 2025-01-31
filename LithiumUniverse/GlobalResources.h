#pragma once
#include <vector>
#include <string>
#include <map>

#include "GameResource.h";

extern std::vector<GameResource> Resources;
extern std::unordered_map<std::string, int> F_GameObjects;

GameResource GetResourceDebug(const std::string FullPath);
GameResource GetResource(const std::string Base, const std::string Path);
GameResource GetResource(const std::string Path);
int GetResourceAssetID(const int ID, const int IfNotFound);
int GetResourceAssetID(const int ID);
int GetResourceID(const int ID, const int IfNotFound);
int GetResourceID(const int ID);
int GetResourceID(const std::string Path, const std::string IfNotFound);

std::string FullPathToComplex(const std::string FullPath);
std::string ComplexToFullPath(const std::string NotFullPath);
std::string GetBaseFromPath(const std::string Path);
void UpdateResources();

void ClearModsResources();
void SaveGameObject(const int i, const std::string ModID, const std::string Name);
int CloneSavedGameObject(const std::string ComplexPath);