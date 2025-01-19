#include <string>

#include "ExplorerActions.h";
#include "GameInstalls.h";
#include "GameData.h";

void End() {
	
}

int Start(char* ExePath) {
	GamePath = RemoveLastFileInPath(FixPath(ExePath));
	return Run();
}