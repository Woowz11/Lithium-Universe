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
	int CharWidth = -1;
	int CharHeight = -1;
	bool Monospace = false;
	std::unordered_map<std::string, int> Chars = {};
	for (auto& [key, value] : FontInfo.items()) {
		if (key != "Error") {
			if (key == "CharWidth" && value.is_number()) {
				CharWidth = value;
			}
			else if (key == "CharHeight" && value.is_number()) {
				CharHeight = value;
			}
			else if (key == "Monospace" && value.is_boolean()) {
				Monospace = value;
			}
			else {
				if (value.is_number()) {
					Chars[key] = value;
				}
				else {
					Warn("FONT", "Failed to get CharPosition from char [" + key + "] info because it is not int! CreateFont(\"" + r + "\",?);");
				}
			}
		}
	}

	if (CharWidth > 0 && CharHeight > 0) {
		std::string ID = GetBaseFromPath(r);
		Font F = Font(r, ID);

		int ErrorChar = 0;
		std::unordered_map<int, int> CharsIDs = {};
		for (auto [CharString, CharPosition] : Chars) {
			if (CharString == "ErrorChar") {
				ErrorChar = CharPosition;
			}
			else {
				if (!CharString.empty()) {
					try {
						auto utf32 = UTF8ToUTF32(CharString);
						if (utf32.size() == 1) {
							uint32_t C = utf32[0];
							CharsIDs[C] = CharPosition;
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
			FontChar Char = FontChar(CharID);
			Char.X = CharPos;
			CharsFinal[CharID] = Char;
		}
		F.Chars = CharsFinal;

		return F;

		Print("FONT", "Font ($$Y" + r + "$$_) $$Gcreated$$_!");
	}
	else {
		Error("FONT", "Unable to load font [$$Y" + r + "$$_] because there are no variables CharWidth or CharHeight, or CharWidth <=0 or CharHeight <= 0! CreateFont(\"" + r + "\",?);");
	}
	return Font("", "");
}