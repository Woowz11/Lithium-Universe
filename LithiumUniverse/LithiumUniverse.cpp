#include <string>

#include "ExplorerActions.h";
#include "GameInstalls.h";
#include "Console.h";

void End() {
	CloseConsole();
}

int Start(char* ExePath) {
	std::string GamePath = RemoveLastFileInPath(FixPath(ExePath));
	InstallConsole(GamePath);
	return Run();
}