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
	std::unordered_map<std::string, std::vector<double>> Chars = {};
	for (auto& [key, value] : FontInfo.items()) {
		if (key != "Error") {
			if (value.size() >= 2 && value.size() <= 3) {
				int CharPositionError = -1;
				double Value2 = 1;

				if (!value[0].is_number_integer()) {
					CharPositionError = 0;
				}
				if (!value[1].is_number_integer()) {
					CharPositionError = 1;
				}
				if (value.size() >= 3) {
					if (!value[2].is_number()) {
						CharPositionError = 2;
					}
					else {
						Value2 = value[2];
					}
				}

				if (CharPositionError == -1) {
					Chars[key] = { value[0] ,value[1], Value2 };
				}
				else {
					Warn("FONT", "Failed to get CharPosition [" + std::to_string(CharPositionError) + "] from char [" + key + "] info because the values ​​in the table are not int format numbers! CreateFont(\"" + r + "\",?);");
				}

			}
			else {
				Warn("FONT", "Failed to get CharPosition from char [" + key + "] info because it should be a table with 3-4 values! CreateFont(\"" + r + "\",?);");
			}
		}
	}

	std::string ID = GetBaseFromPath(r);
	Font F = Font(r, ID);

	std::vector<double> ErrorChar = {0,0,1};
	std::unordered_map<int, std::vector<double>> CharsIDs = {};
	for (auto [CharString, CharPosition] : Chars) {
		if (CharString == "") {
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
	int MaxX = -1;
	int MaxY = -1;
	for (auto [CharID, CharPos] : CharsIDs) {
		if (CharPos[0] >= 0 && CharPos[1] >= 0 && CharPos[2] > 0 && CharPos[2] <= 1) {
			FontChar Char = FontChar(CharID);
			Char.X = CharPos[0];
			Char.Y = CharPos[1];
			if (Char.X > MaxX) { MaxX = Char.X; }
			if (Char.Y > MaxY) { MaxY = Char.Y; }
			Char.W = CharPos[2];
			CharsFinal[CharID] = Char;
		}
		else {
			Warn("FONT", "Failed to convert character (" + std::to_string(CharID) + ") to position, because the position [" + ToStringVec2(glm::vec2(CharPos[0], CharPos[1])) + "] or width [" + std::to_string(CharPos[2]) + "] of the character is not correct! CreateFont(\"" + r + "\",?);");
		}
	}
	F.Chars = CharsFinal;
	F.MaxX = MaxX;
	F.MaxY = MaxY;

	Print("FONT", "Font ($$Y" + r + "$$_) $$Gcreated$$_!");

	return F;
}