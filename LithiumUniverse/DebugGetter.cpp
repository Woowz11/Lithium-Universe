#include <fstream>
#include <string>

#include "ExplorerActions.h";
#include "StringActions.h";
#include "BaseConstants.h";
#include "GameData.h";
#include "Texture.h";

std::string GameName = "";

std::string DebugPath = "";

std::string DebugFile_Textures = "";

std::string BoldPath(const std::string& Path) {
	size_t colonPos = Path.find(':');
	if (colonPos != std::string::npos) {
		std::string firstPart = Path.substr(0, colonPos);
		return "**" + firstPart + "**" + Path.substr(colonPos);
	}
	else {
		WarnSerious("DebugGetter","Couldn't make base bold on the way because it wasn't found! BoldPath(\"" + Path + "\");");
	}
	return Path;
}

void InstallDebug() {
	if (DeveloperVersion) {
		DebugPath = AddFileToPath(GamePath, "Debug");
		CreateFolder(DebugPath);

		GameName = "Lithium Universe " + GetGameVersion();

		DebugFile_Textures = DebugPath + "/Textures.md";
		CreateFileLongWay(DebugFile_Textures);
	}
}

void StartDebug() {
	if (DeveloperVersion) {
		/* Текстуры */
		AddToFileLongWay(DebugFile_Textures,
			"# " + GameName +
			"\n## Texture files loading list!\n" +
			"|" + " " + "|" + "ID " + "|" + FillString("Atlas", ' ', 10, false) + "|" + FillString("Texture Path", ' ', 100, false) + "|\n" +
			"|" + "-" + "|:" + FillString("", '-', 3 - 2, false) + ":|:" + FillString("", '-', 10 - 2, false) + ":|:" + FillString("", '-', 100 - 1, false) + "|\n"
		);
	}
}

void FinishLoadingResources() {
	if (DeveloperVersion) {
		for (Texture T : Texturies) {
			AddToFileLongWay(DebugFile_Textures,
				"|" + std::string(T.Error ? "E" : "C") + "|" + FillString(std::to_string(T.ID), ' ', 3, false) + "|" + FillString(T.AtlasName, ' ', 10, false) + "|" + FillString(BoldPath(T.Path), ' ', 100, false) + "|\n");
		}

		AddToFileLongWay(DebugFile_Textures, "\n## Texture files showing\n|ID |" +
		           FillString("Texture", ' ', 113,     false) + "|\n"+
		"|:-:|:" + FillString("",        '-', 113 - 1, false) + "|\n");

		for (Texture T : Texturies) {
			AddToFileLongWay(DebugFile_Textures,
				"|" + FillString(std::to_string(T.ID), ' ', 3, false) + "|" + FillString("![](" + T.FullPath + ")", ' ', 113, false) + "|\n");
		}
	}
}

void ClearDebug() {
	if (DeveloperVersion) {
		if (HasFile(DebugFile_Textures)) {
			ClearFileLongWay(DebugFile_Textures);
		}
	}
}