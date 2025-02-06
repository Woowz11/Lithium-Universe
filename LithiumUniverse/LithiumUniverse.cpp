#include <string>

#include "ExplorerActions.h";
#include "StringActions.h";
#include "GameInstalls.h";
#include "OSActions.h";
#include "GameData.h";

void End() {
	
}

int Start(char* ExePath) {
	if (!OnlyASCIIChars(ExePath)) {
		CrashMessageBox(L"The path to the game must contain only ASCII characters!");
		return -1;
	}

	GamePath = RemoveLastFileInPath(FixPath(ExePath));
	std::string DLLPath = AddFileToPath(GamePath, "DLL");

	if (!HasFile(AddFileToPath(DLLPath, "lua54.dll"))) {
		NotFoundDLLMessageBox(L"lua54.dll");
		return -1;
	}

	return Run();
}