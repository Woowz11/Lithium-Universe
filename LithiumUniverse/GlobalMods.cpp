#include <string>
#include <vector>
#include <queue>

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
		std::string FrongType = "";
		if (ModInfo["Error"] == true) {
			Error("MODS","Unable to load modification because file $$Y" + ModID + ".lu_mod$$_ has an error! LoadMod(\"" + FullPath + "\");");
		}
		else {
			if (ModInfo.contains("Conflict")) {
				if (!ModInfo["Conflict"].is_array()) {
					FrongType = "Conflict";
				}
				else {
					for (auto a : ModInfo["Conflict"]) {
						if (!a.is_string()) {
							FrongType = "Conflict";
						}
					}
				}
			}
			else {
				NotHasThis = "Conflict";
			}

			if (ModInfo.contains("Requires")) {
				if (!ModInfo["Requires"].is_array()) {
					FrongType = "Requires";
				}
				else {
					for (auto a : ModInfo["Requires"]) {
						if (!a.is_string()) {
							FrongType = "Requires";
						}
					}
				}
			}
			else {
				NotHasThis = "Requires";
			}

			if (ModInfo.contains("Description")) {
				if (!ModInfo["Description"].is_string()) {
					FrongType = "Description";
				}
			}
			else {
				NotHasThis = "Description";
			}

			if (ModInfo.contains("Version")) {
				if (!ModInfo["Version"].is_string()) {
					FrongType = "Version";
				}
			}
			else {
				NotHasThis = "Version";
			}

			if (ModInfo.contains("Author")) {
				if (!ModInfo["Author"].is_string()) {
					FrongType = "Author";
				}
			}
			else {
				NotHasThis = "Author";
			}

			if (ModInfo.contains("Name")) {
				if (!ModInfo["Name"].is_string()) {
					FrongType = "Name";
				}
			}
			else {
				NotHasThis = "Name";
			}

			if (ModInfo.contains("MainScript")) {
				if (!ModInfo["MainScript"].is_string()) {
					FrongType = "MainScript";
				}
			}

			if (NotHasThis == "") {
				if (FrongType == "") {
					GameMod GM = GameMod(FullPath, ModID);
					GM.Name = ModInfo["Name"];
					if (ModInfo.contains("MainScript")) {
						GM.MainScript = ModInfo["MainScript"];
					}
					GM.Author = ModInfo["Author"];
					GM.Version = ModInfo["Version"];
					GM.Desc = ModInfo["Description"];
					for (std::string a : ModInfo["Requires"]) {
						GM.Requires.push_back(a);
					}
					for (std::string a : ModInfo["Conflict"]) {
						GM.Conflict.push_back(a);
					}
					Mods.push_back(GM);
					Print("MODS", "Mod $$Y" + ModID + " " + GM.Version + "$$_ $$Gsuccessfully$$_ loaded!");
				}
				else {
					Error("MODS", "Unable to load modification because the field [$$Y" + FrongType + "$$_] is of the wrong type (not string (or other type)) in the file $$Y" + ModID + ".lu_mod$$_! LoadMod(\"" + FullPath + "\");");
				}
			}
			else {
				Error("MODS", "Unable to load modification because the field [$$Y" + NotHasThis + "$$_] is missing in the file $$Y" + ModID + ".lu_mod$$_! LoadMod(\"" + FullPath + "\");");
			}
		}
	}
	else {
		Error("MODS","Unable to load modification because its main file $$Y" + ModID + ".lu_mod$$_ was not found! LoadMod(\"" + FullPath + "\");");
	}
}

size_t TotalMods = 0;
void LoadMods() {
	Mods.clear();
	TotalMods = 0;
	std::vector<std::string> ModsFoldes = GetFolders(ModsPath);
	for (const auto s : ModsFoldes) {
		TotalMods++;
		LoadMod(s);
	}

	Print("MODS", "Mods ($$" + std::string(Mods.size() == TotalMods ? "G" : (Mods.size() == 0 ? "R" : "Y")) + std::to_string(Mods.size()) + "/" + std::to_string(TotalMods) + "$$_) loaded!");

	Print("MODS", "Checking mod compatibility and sorting!");

	for (GameMod& Mod : Mods) {
		std::string Requests = "";
		for (std::string a : Mod.Requires) {
			auto it = std::find_if(Mods.begin(), Mods.end(), [&a](const GameMod& M) {
				return M.ID == a;
			});

			if (it == Mods.end()) {
				Mod.NeedMod = a;
			}
			std::string HasModColor = (it==Mods.end()? "$$R" : "$$G");

			if (Requests.empty()) {
				Requests = "requires: \"" + HasModColor + a;
			}
			else {
				Requests += "$$_\", \"" + HasModColor + a;
			}
		}
		if (Requests.empty()) {
			Requests = "does $$Gnot require anything$$_";
		}
		else {
			Requests += "$$_\"";
		}

		std::string Incompatible;
		for (std::string a : Mod.Conflict) {
			auto it = std::find_if(Mods.begin(), Mods.end(), [&a](const GameMod& M) {
				return M.ID == a;
			});

			if (it != Mods.end()) {
				Mod.NoNeedMod = a;
			}
			std::string HasModColor = (it == Mods.end() ? "$$G" : "$$R");

			if (Incompatible.empty()) {
				Incompatible = "conflict: \"" + HasModColor + a;
			}
			else {
				Incompatible += "$$_\", \"" + HasModColor + a;
			}
		}
		if (Incompatible.empty()) {
			Incompatible = "does $$Gnot conflict anything$$_";
		}
		else {
			Incompatible += "$$_\"";
		}

		Print("MODS", "Mod $$Y" + Mod.ID + " " + Mod.Version + "$$_ " + Requests + "; " + Incompatible);
	}

	std::unordered_map<std::string, GameMod> modMap;
	std::unordered_map<std::string, std::vector<std::string>> graph;
	std::unordered_map<std::string, int> inDegree;

	for (const auto& mod : Mods) {
		modMap[mod.ID] = mod;
		inDegree[mod.ID] = 0;
	}

	for (const auto& mod : Mods) {
		for (const auto& req : mod.Requires) {
			graph[req].push_back(mod.ID);
			inDegree[mod.ID]++;
		}
	}

	std::queue<std::string> queue;
	for (const auto& mod : Mods) {
		if (inDegree[mod.ID] == 0) {
			queue.push(mod.ID);
		}
	}

	std::vector<GameMod> sortedMods;
	while (!queue.empty()) {
		std::string currentModID = queue.front();
		queue.pop();

		sortedMods.push_back(modMap[currentModID]);


		for (const auto& dependentModID : graph[currentModID]) {
			inDegree[dependentModID]--;
			if (inDegree[dependentModID] == 0) {
				queue.push(dependentModID);
			}
		}
	}

	if (sortedMods.size() != Mods.size()) {
		Error("MODS", "$$RCyclic mods dependency detected!");
		Mods.clear();
	}
	else {
		Mods = sortedMods;
	}

	Print("MODS", "General information about mods!");

	for (const GameMod& Mod : Mods) {
		std::string Info = "";

		if (!Mod.NeedMod.empty()) {
			Info = "$$RRequires mod " + Mod.NeedMod + "!";
		}
		if (!Mod.NoNeedMod.empty()) {
			Info = "$$RConflicts with " + Mod.NoNeedMod + "!";
		}

		if (Info.empty()) {
			Info = "$$GLoaded!";
		}
		Print("MODS", "Mod $$Y" + Mod.ID + " " + Mod.Version + "$$_: " + Info);
	}
}

void CheckMods() {
	Print("MODS", "Start loading mods...");

	ModsPath = AddFileToPath(GamePath, "Mods");
	CreateFolder(ModsPath);

	LoadMods();
}

void StopMods() {
	UnloadLua();
}

void RunMods() {
	ClearModsResources();
	for (GameMod GM : Mods) {
		if (!GM.MainScript.empty()) {
			LoadLua(GM);
			RunScript(GM.MainScript);
		}
	}

	for (const sol::function& F : LUA_Events_GameObjectLoading) {
		F();
	}

	for (const sol::function& F : LUA_Events_UILoading) {
		F();
	}
}