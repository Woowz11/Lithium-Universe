#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include "ExplorerActions.h";
#include "GameResource.h";
#include "GameData.h";
#include "Console.h";
#include "Texture.h";
#include "Shader.h";

const std::string BaseID              = "Base";
const std::string VanillaID           = "Vanilla";
const std::string VanillaPhysicID     = "VanillaPhysic";
const std::string VanillaControllerID = "VanillaController";

/* Все ресурсы игры */
std::vector<GameResource> Resources = {};

/* Пути ресурсов */
std::vector<std::string> F_Other    = {};
std::vector<std::string> F_Scripts  = {};
std::vector<std::string> F_Shaders  = {};
std::vector<std::string> F_Textures = {};
std::vector<std::string> F_Sounds   = {};

/* Путь до текстуры ошибки */
std::string ErrorTexturePath = "";

/* Получить ресурс по полному пути */
GameResource GetResourceDebug(std::string FullPath) {
	auto it = std::find_if(Resources.begin(), Resources.end(),
		[&FullPath](const GameResource& GR) { return GR.FullPath == FullPath; });

	if (it != Resources.end()) {
		return Resources[std::distance(Resources.begin(), it)];
	}
	else {
		Error("RES", "Resource not found at path! GetResourceDebug(\"" + FullPath + "\");");
		return GetResourceDebug(ErrorTexturePath);
	}
}

/* Получить ресурс по пути */
GameResource GetResource(std::string Base, std::string Path) {
	return GetResourceDebug(GamePath + "/" + (Base == BaseID ? "Resources" : Base) + "/" + Path);
}

/* Получить айди ресурса */
int GetResourceID(int ID, int IfNotFound) {
#ifdef NDEBUG
	return Resources[ID];
#else
	if (ID >= 0 && ID < Resources.size()) {
		GameResource GR = Resources[ID];
		if (!GR.Deleted) {
			return ID;
		}
		else {
			Error("RES", "Unable to get resource because it has been deleted! GetResourceID(" + std::to_string(ID) + "," + std::to_string(IfNotFound) + ");");
		}
	}
	else {
		Error("RES", "Unable to get resource because it is not in the array of all resources! GetResourceID(" + std::to_string(ID) + "," + std::to_string(IfNotFound) + ");");
	}
	return IfNotFound;
#endif
}
int GetResourceID(int ID) {
	return GetResourceID(ID, GetResourceDebug(ErrorTexturePath).ID);
}

/* Получить айди ассета ресурса */
int GetResourceAssetID(int ID, int IfNotFound) {
	return Resources[GetResourceID(ID, IfNotFound)].AssetID;
}
int GetResourceAssetID(int ID) {
	return GetResourceAssetID(ID, GetResourceDebug(ErrorTexturePath).ID);
}

/* Определить ресурс по типу */
void DefineResource(std::string Path) {
	std::string type = GetFileType(Path);

	if (type == "png") {
		F_Textures.push_back(Path);
	}
	else if (type == "shader") {
		F_Shaders.push_back(Path);
	}
	else {
		F_Other.push_back(Path);
	}
}

void CreateNewGameResourceOrSkip(std::string FullPath_, GameResourceType Type_, int ID_, int AssetID_) {
	auto it = std::find_if(Resources.begin(), Resources.end(),
		[&FullPath_](const GameResource& GR) { return GR.FullPath == FullPath_; });

	if (it != Resources.end()) {
		GameResource& GR = Resources[std::distance(Resources.begin(), it)];
		GR.Deleted = false;
		GR.AssetID = AssetID_;
	}
	else {
		GameResource GR = GameResource(FullPath_, Type_, ID_, AssetID_);
		Resources.push_back(GR);
	}
}

void GetAllFilesInGamePath() {
	std::vector<std::string> F_Scripts  = {};
	std::vector<std::string> F_Shaders  = {};
	std::vector<std::string> F_Textures = {};
	std::vector<std::string> F_Sounds   = {};
	std::vector<std::string> F_Other    = {};

	std::string ResourcesPath = GamePath + "/Resources";
	std::string ModsPath      = GamePath + "/Mods";

	std::vector<std::string> R_Resources = GetFilesIncludeSubFolders(ResourcesPath);
	std::vector<std::string> R_Mods      = GetFilesIncludeSubFolders(ModsPath);

	for (std::string r : R_Resources) {
		DefineResource(r);
	}

	for (std::string r : R_Mods) {
		DefineResource(r);
	}
}

void UpdateR_Other() {
	for (std::string r : F_Other) {
		CreateNewGameResourceOrSkip(r, GR_Other, Resources.size(), -1);
	}
	Print("RES", "Other loaded!");
}

void UpdateR_Scripts() {
	for (std::string r : F_Scripts) {

	}
	Print("RES", "Scripts loaded!");
}

void UpdateR_Shaders() {
	for (std::string r : F_Shaders) {

		CreateNewGameResourceOrSkip(r, GR_Shader, Resources.size(), -1);
	}
	Print("RES", "Shaders loaded!");
}

void RemoveR_Textures() {
	F_Textures = {};
	for (Texture T : Texturies) {
		DeleteTexture(T);
	}
	Texturies = {};
}

void UpdateR_Textures() {
	ErrorTexturePath = GamePath + "/Resources/Textures/Error/NotFound.png";
	std::string Target = ErrorTexturePath;
	std::sort(F_Textures.begin(), F_Textures.end(), [&Target](const std::string& a, const std::string& b) {
		if (a == Target) return true;
		if (b == Target) return false;
		return false;
	});

	for (std::string r : F_Textures) {
		Texture T = CreateTexture(r, "Test");

		CreateNewGameResourceOrSkip(r, GR_Texture, Resources.size(), T.ID);
	}
	Print("REs", "Textures loaded!");
}

void UpdateR_Sounds() {
	for (std::string r : F_Sounds) {

	}
	Print("RES", "Sounds loaded!");
}

void UpdateResources() {
	std::string VanillaTexturesFolder = AddFileToPath(AddFileToPath(GamePath, "Resources"), "Textures");
	CreateFolder(VanillaTexturesFolder);

	Print("RES", "Loading resources has started...");

	RemoveR_Textures();

	GetAllFilesInGamePath();
	for (auto p : Resources) {
		p.Deleted = true;
	}
	UpdateR_Other();
	UpdateR_Scripts();
	UpdateR_Shaders();
	UpdateR_Textures();
	UpdateR_Sounds();
	Print("RES", "Resources updated!");
}