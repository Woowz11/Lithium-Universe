#include <string>
#include <vector>

#include "ExplorerActions.h";
#include "GameData.h";
#include "Console.h";

std::string ModsPath = "";

void LoadMods() {
	std::vector<std::string> ModsFoldes = GetFolders(ModsPath);
	for (auto s : ModsFoldes) {
		Print(":)", "MODS -> " + s);
	}
}

void CheckMods() {
	Print("MODS", "Start loading mods...");

	ModsPath = AddFileToPath(GamePath, "Mods");
	CreateFolder(ModsPath);

	LoadMods();

	Print("MODS", "Mods loaded!");
}