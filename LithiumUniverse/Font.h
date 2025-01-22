#ifndef FONT_H
#define FONT_H

#include <unordered_map>
#include <string>
#include <vector>

#include "nlohmann/json.hpp";
using json = nlohmann::json;

#include "FontChar.h";

class Font {
public:
	std::string FullPath;
	std::string ID;
	std::unordered_map<int, FontChar> Chars = {};

	Font(std::string FullPath_, std::string ID_) {
		FullPath = FullPath_;
		ID = ID_;
	}
};

extern std::vector<Font> Fonts;

Font CreateFont(std::string r, json FontInfo);

#endif