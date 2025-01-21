#ifndef GAME_MOD_H
#define GAME_MOD_H

#include <string>

class GameMod {
public:
	std::string FullPath;
	std::string Name = "Unknown mod!";
	std::string MainScript = "";
	std::string Version = "0.0";
	std::string Author = "Anonymous";
	std::string Desc = "";

	GameMod(std::string FullPath_) {
		FullPath = FullPath_;
	}
};

#endif