#include <fstream>
#include <string>

#include "ExplorerActions.h";
#include "StringActions.h";
#include "GameData.h";
#include "Texture.h";

std::string DebugPath = "";

std::string DebugFile_Textures = "";

void DebugLog_LoadTexture(Texture& T, bool success) {
	if (DeveloperVersion) {
		AddToFileLongWay(DebugFile_Textures, 
			"|" + FillString(std::to_string(T.ID), ' ', 3, false) + "|" + FillString(T.AtlasName, ' ', 10, false) + "|" + FillString(T.FullPath, ' ', 100, false) + "|\n");
	}
}

void InstallDebug() {
	if (DeveloperVersion) {
		DebugPath = AddFileToPath(GamePath, "Debug");
		CreateFolder(DebugPath);

		DebugFile_Textures = DebugPath + "/TextureLoading.md";
		CreateFile_(DebugFile_Textures);

		ClearFileLongWay(DebugFile_Textures);
		AddToFileLongWay(DebugFile_Textures,
			"|"  + FillString("ID",' ', 3,   false) +  "|"  + FillString("Atlas", ' ', 10,   false) +  "|"  + FillString("Texture Path", ' ', 100,   false) + "|\n"+
			"|:" + FillString("",  '-', 3-2, false) + ":|:" + FillString("",      '-', 10-2, false) + ":|:" + FillString("",             '-', 100-1, false) + "|\n"
		);
	}
}

void CloseDebug() {
	if (DeveloperVersion) {
		
	}
}