#include <string>
#include <vector>
#include <map>

#include "ExplorerActions.h";
#include "GameResource.h";
#include "GameData.h";
#include "Console.h";
#include "Texture.h";
#include "Shader.h";

const std::string VanillaID           = "Vanilla";
const std::string VanillaPhysicID     = "VanillaPhysic";
const std::string VanillaControllerID = "VanillaController";

/* Все ресурсы игры */
std::vector<GameResource> Resources = {};

/* Пути ресурсов */
std::vector<std::string> F_Scripts  = {};
std::vector<std::string> F_Shaders  = {};
std::vector<std::string> F_Textures = {};
std::vector<std::string> F_Sounds   = {};
std::vector<std::string> F_Other    = {};

/* Ошибочный ресурс */
GameResource GameResourceError = GameResource("", "", "", GR_Other, -1, -1);

/* Получить ресурс по полному пути */
GameResource GetResourceDebug(std::string FullPath) {
	auto it = std::find_if(Resources.begin(), Resources.end(),
		[&FullPath](const GameResource& GR) { return GR.FullPath == FullPath; });

	if (it != Resources.end()) {
		return Resources[std::distance(Resources.begin(), it)];
	}
	else {
		Error("RES", "Resource not found at path! GetResourceDebug(\"" + FullPath + "\");");
		return GameResourceError;
	}
}

/* Определить ресурс по типу */
void DefineResource(std::string Path) {
	std::string type = GetFileType(Path);

	if (type == "png") {
		F_Textures.push_back(Path);
	}
	else {
		F_Other.push_back(Path);
	}
}

GameResource CreateNewGameResourceOrSkip(std::string FullPath_, GameResourceType Type_, int ID_, int AssetID_) {
	auto it = std::find_if(Resources.begin(), Resources.end(),
		[&FullPath_](const GameResource& GR) { return GR.FullPath == FullPath_; });

	std::string Base_ = "test";
	std::string Path_ = "Textures/Default2.png";

	if (it != Resources.end()) {
		GameResource GR = Resources[std::distance(Resources.begin(), it)];
		GR.Deleted = false;
		GR.AssetID = AssetID_;
		return GR;
	}
	else {
		GameResource GR = GameResource(FullPath_, Base_, Path_, Type_, ID_, AssetID_);
		Resources.push_back(GR);
		return GR;
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

void UpdateR_Scripts() {
	for (std::string r : F_Scripts) {

	}
	Print("R/SCR.", "Loaded!");
}

void UpdateR_Shaders() {
	for (std::string r : F_Shaders) {

	}
	Print("R/SHDR.", "Loaded!");
}

void RemoveR_Textures() {
	for (Texture T : Texturies) {
		DeleteTexture(T.ID);
	}
	Texturies = {};
}

void UpdateR_Textures() {
	for (std::string r : F_Textures) {
		Texture T = CreateTexture(r, "Test");

		CreateNewGameResourceOrSkip(r, GR_Texture, Resources.size(), T.ID);
	}
	Print("R/TXTR.", "Loaded!");
}

void UpdateR_Sounds() {
	for (std::string r : F_Sounds) {

	}
	Print("R/SND.", "Loaded!");
}

void UpdateR_Other() {
	for (std::string r : F_Other) {

	}
	Print("R/OTHR.","Loaded!");
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
	UpdateR_Scripts();
	UpdateR_Shaders();
	UpdateR_Textures();
	UpdateR_Sounds();
	UpdateR_Other();
	Print("RES", "Resources updated!");
}