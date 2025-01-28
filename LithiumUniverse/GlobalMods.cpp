#include <string>
#include <vector>

#include "nlohmann/json.hpp";
using json = nlohmann::json;

#include "GlobalResources.h";
#include "ExplorerActions.h";
#include "GlobalLua.h";
#include "GameData.h";
#include "Console.h";
#include "GameMod.h";

/* Путь до папки с модами */
std::string ModsPath = "";

/* Список загруженных модов */
std::vector<GameMod> Mods = {};

void LoadMod(std::string FullPath) {
	std::string ModID = GetFolderName(FullPath);
	std::string ModInfoPath = FullPath + "/" + ModID + ".lu_mod";
	if (HasFile(ModInfoPath)) {
		json ModInfo = ReadJson(ModInfoPath);
		std::string NotHasThis = "";
		if (ModInfo["Error"] == true) {
			Error("MODS","Unable to load modification because file $$Y" + ModID + ".lu_mod$$_ has an error! LoadMod(\"" + FullPath + "\");");
		}
		else {
			if (!ModInfo.contains("Incompatible")) {
				NotHasThis = "Incompatible";
			}
			if (!ModInfo.contains("Compatibility")) {
				NotHasThis = "Compatibility";
			}
			if (!ModInfo.contains("Description")) {
				NotHasThis = "Description";
			}
			if (!ModInfo.contains("Version")) {
				NotHasThis = "Version";
			}
			if (!ModInfo.contains("Author")) {
				NotHasThis = "Author";
			}
			if (!ModInfo.contains("MainScript")) {
				NotHasThis = "MainScript";
			}
			if (!ModInfo.contains("Name")) {
				NotHasThis = "Name";
			}

			if (NotHasThis == "") {
				GameMod GM = GameMod(FullPath, ModID);
				GM.Name       = ModInfo["Name"];
				GM.MainScript = ModInfo["MainScript"];
				GM.Author     = ModInfo["Author"];
				GM.Version    = ModInfo["Version"];
				GM.Desc       = ModInfo["Description"];
				Mods.push_back(GM);
				Print("MODS", "Mod $$Y" + ModID + " " + GM.Version + "$$_ $$Gsuccessfully$$_ loaded!");
			}
			else {
				Error("MODS", "Unable to load modification because the field $$Y" + NotHasThis + "$$_ is missing in the file $$Y" + ModID + ".lu_mod$$_! LoadMod(\"" + FullPath + "\");");
			}
		}
	}
	else {
		Error("MODS","Unable to load modification because its main file $$Y" + ModID + ".lu_mod$$_ was not found! LoadMod(\"" + FullPath + "\");");
	}
}

size_t TotalMods = 0;
void LoadMods() {
	Mods = {};
	TotalMods = 0;
	std::vector<std::string> ModsFoldes = GetFolders(ModsPath);
	for (auto s : ModsFoldes) {
		TotalMods++;
		LoadMod(s);
	}
}

void CheckMods() {
	Print("MODS", "Start loading mods...");

	ModsPath = AddFileToPath(GamePath, "Mods");
	CreateFolder(ModsPath);

	LoadMods();

	Print("MODS", "Mods ($$" + std::string(Mods.size()==TotalMods ? "G" : (Mods.size()==0 ? "R" : "Y")) + std::to_string(Mods.size()) + "/" + std::to_string(TotalMods) + "$$_) loaded!");
}

void StopMods() {
	UnloadLua();
}

void RunMods() {
	ClearModsResources();
	for (GameMod GM : Mods) {
		LoadLua(GM);
		RunScript(GM.MainScript);
	}

	for (auto F : LUA_Events_GameObjectLoading) {
		F();
	}
}