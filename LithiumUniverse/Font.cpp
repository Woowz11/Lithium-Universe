#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>
#include <locale>

#include "nlohmann/json.hpp";
using json = nlohmann::json;

#include "GlobalResources.h";
#include "StringActions.h";
#include "Console.h";
#include "Texture.h";
#include "Font.h";

std::vector<Font> Fonts = {};

Font CreateFont(std::string r ,json FontInfo) {
	std::unordered_map<std::string, std::vector<int>> Chars = {};
	for (auto& [key, value] : FontInfo.items()) {
		if (key != "Error") {
			if (value.size() == 4) {
				if (value[0].is_number_integer() && value[1].is_number_integer() && value[2].is_number_integer() && value[3].is_number_integer()) {
					Chars[key] = { value[0] ,value[1] ,value[2] ,value[3] };
				}
				else {
					Warn("FONT", "Failed to get CharPosition from char [" + key + "] info because the values ​​in the table are not int format numbers! CreateFont(\"" + r + "\",?);");
				}
			}
			else {
				Warn("FONT", "Failed to get CharPosition from char [" + key + "] info because it should be a table with 4 values! CreateFont(\"" + r + "\",?);");
			}
		}
	}

	std::string ID = GetBaseFromPath(r);
	Font F = Font(r, ID);

	std::vector<int> ErrorChar = {0,0,1,1};
	std::unordered_map<int, std::vector<int>> CharsIDs = {};
	for (auto [CharString, CharPosition] : Chars) {
		if (CharString == "ErrorChar") {
			ErrorChar = CharPosition;
		}
		else {
			if (!CharString.empty()) {
				try {
					std::u32string utf32 = std::u32string(CharString.begin(), CharString.end());
					if (utf32.size() == 1) {
						CharsIDs[utf32[0]] = CharPosition;
					}
					else {
						Warn("FONT", "Failed to convert character '" + CharString + "' to position, because the string must consist of one character! CreateFont(\"" + r + "\",?);");
					}
				}
				catch (...) {
					Warn("FONT", "Failed to convert character '" + CharString + "' to position! CreateFont(\"" + r + "\",?);");
				}
			}
			else {
				Warn("FONT", "Failed to convert character '" + CharString + "' to position, because the string empty! CreateFont(\"" + r + "\",?);");
			}
		}
	}
	CharsIDs[-1] = ErrorChar;

	std::unordered_map<int, FontChar> CharsFinal = {};
	for (auto [CharID, CharPos] : CharsIDs) {
		if (CharPos[0] >= 0 && CharPos[1] >= 0 && CharPos[2] > 0 && CharPos[3] > 0) {
			FontChar Char = FontChar(CharID);
			Char.X = CharPos[0];
			Char.Y = CharPos[1];
			Char.W = CharPos[2];
			Char.H = CharPos[3];
			CharsFinal[CharID] = Char;
		}
		else {
			Warn("FONT", "Failed to convert character (" + std::to_string(CharID) + ") to position, because the position [" + ToStringVec2(glm::vec2(CharPos[0], CharPos[1])) + "] or size [" + ToStringVec2(glm::vec2(CharPos[2], CharPos[3])) + "] of the character is not correct! CreateFont(\"" + r + "\",?);");
		}
	}
	F.Chars = CharsFinal;

	Print("FONT", "Font ($$Y" + r + "$$_) $$Gcreated$$_!");

	return F;
}