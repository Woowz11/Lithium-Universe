#include <GLM/glm.hpp>

#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include "GameObject.h";
#include "Console.h";

/* Скрипт позволяющий более углублённо управлять строками */

const std::string Base_SA = "StringActions";

/* Заменить в строке что-то на что-то */
std::string ReplaceStringToString(const std::string Target, const std::string This, const std::string ToThat) {
	std::string Result = Target;
	size_t pos = 0;
	while ((pos = Result.find(This, pos)) != std::string::npos) {
		Result.replace(pos, This.length(), ToThat);
		pos += ToThat.length();
	}
	return Result;
}

/* Убрать пробелы из начала и конца */
std::string Trim(const std::string& Target) {
    size_t first = Target.find_first_not_of(" \t\n\r");
    size_t last = Target.find_last_not_of(" \t\n\r");

    if (first == std::string::npos || last == std::string::npos) {
        return "";
    }

    return Target.substr(first, last - first + 1);
}

/* Заполнить строку символами */
std::string FillString(const std::string& Target, const char Symbol, const int Length, const bool ToLeft) {
	if (Target.length() >= Length) {
		return Target;
	}

	int fillLength = Length - Target.length();
	std::string filledString(fillLength, Symbol);
	return ToLeft ? filledString + Target : Target + filledString;
}

/* Заменить все символы в строке на другие символы */
std::string ReplaceCharsToChars(const std::string& Target, const char ToThis, const char ToThat) {
	std::string Result = Target;
	std::replace(Result.begin(), Result.end(), ToThis, ToThat);
	return Result;
}

/* Заменить все символы в строке на строку */
std::string ReplaceCharsToString(const std::string& Target, const char ToThis, const std::string ToThat) {
	std::string Result = Target;
	std::string Char(1, ToThis);
	size_t pos = 0;
	while ((pos = Result.find(Char, pos)) != std::string::npos) {
		Result.replace(pos, Char.length(), ToThat);
		pos += ToThat.length();
	}
	return Result;
}

/* Удалить часть строки */
std::string RemoveStringPart(const std::string& Target, const int Length) {
	std::string Result = Target;
	if (Result.length() > Length) {
		Result = Result.substr(0, Length);
	}
	return Result;
}

/* Повторить строку несколько раз */
std::string RepeatString(const std::string& Target, const int CoutRepeat) {
	if (CoutRepeat <= 0) { return ""; }

	std::string Result;
	Result.reserve(Target.size() * CoutRepeat);

	for (int i = 0; i < CoutRepeat; i++) {
		Result += Target;
	}

	return Result;
}

/* Строка начинается с StartWith? */
bool StringStartWith(const std::string& Target, const std::string StartWith) {
	return Target.find(StartWith) == 0;
}

/* Узнать кол-во линий в строке */
int GetLinesCountFromString(const std::string& Target) {
	int lines = std::count(Target.begin(), Target.end(), '\n');
	if (!Target.empty() && Target.back() != '\n') {
		lines++;
	}
	return lines;
}

/* Получить линию строки из строки */
std::string GetLineFromString(const std::string& Target, const int SelectedLine) {
	if (SelectedLine <= 0) {
		return "";
	}

	std::istringstream Stream(Target);
	std::string Line;
	int CurrentLine = 0;

	while (std::getline(Stream, Line)) {
		CurrentLine++;
		if (CurrentLine == SelectedLine) {
			return Line;
		}
	}

	return "";
}

/* Получить последний символ строки */
char GetLastSymbol(const std::string& Target) {
	if (Target.empty()) {
		return ' ';
	}
	return Target.back();
}

/* Удалить последний символ строки */
std::string RemoveLastSymbol(const std::string& Target) {
	if (Target.empty()) {
		Warn(Base_SA, "Cannot delete the last character in a string because the string is empty! RemoveLastSymbol(\"" + Target + "\");");
		return Target;
	}
	else {
		std::string Result = Target;
		Result.pop_back();
		return Result;
	}
}

/* Удалить первый символ строки */
std::string RemoveFirstSymbol(const std::string& Target) {
	if (Target.empty()) {
		Warn(Base_SA, "Cannot delete the first character in a string because the string is empty! RemoveFirstSymbol(\"" + Target + "\");");
		return Target;
	}
	else {
		return Target.substr(1, Target.length()-1);
	}
}

/* Конвертировать строку в массив чисел */
std::vector<int> StringToNumbers(const std::string Target) {
	std::vector<int> result = {};
	for (size_t i = 0; i < Target.size(); i++) {
		result.push_back(Target[i]);
	}
	return result;
}

/* Конвертировать строку символ в число */
int ConvertStringSymbolToNumber(const std::string Symbol) {
	if (Symbol.empty()) {
		Error(Base_SA, "Cannot convert string-symbol '" + Symbol + "' to number because symbol empty! ConvertStringSymbolToNumber(\"" + Symbol + "\");");
		return -1;
	}

	unsigned char byte1 = static_cast<unsigned char>(Symbol[0]);

	if ((byte1 & 0b10000000) == 0b00000000) {
		return byte1;
	}
	else if ((byte1 & 0b11100000) == 0b11000000) {
		if (Symbol.size() < 2) {
			Error(Base_SA, "Cannot convert string-symbol '" + Symbol + "' to number because the symbol expected 2 bytes, got " + std::to_string(Symbol.size()) + "! ConvertStringSymbolToNumber(\"" + Symbol + "\");");
		}
		unsigned char byte2 = static_cast<unsigned char>(Symbol[1]);
		return ((byte1 & 0b00011111) << 6) | (byte2 & 0b00111111);
	}
	else if ((byte1 & 0b11110000) == 0b11100000) {
		if (Symbol.size() < 3) {
			Error(Base_SA, "Cannot convert string-symbol '" + Symbol + "' to number because the symbol expected 3 bytes, got " + std::to_string(Symbol.size()) + "! ConvertStringSymbolToNumber(\"" + Symbol + "\");");
		}
		unsigned char byte2 = static_cast<unsigned char>(Symbol[1]);
		unsigned char byte3 = static_cast<unsigned char>(Symbol[2]);
		return ((byte1 & 0b00001111) << 12) | ((byte2 & 0b00111111) << 6) | (byte3 & 0b00111111);
	}
	else if ((byte1 & 0b11111000) == 0b11110000) {
		if (Symbol.size() < 4) {
			Error(Base_SA, "Cannot convert string-symbol '" + Symbol + "' to number because the symbol expected 4 bytes, got " + std::to_string(Symbol.size()) + "! ConvertStringSymbolToNumber(\"" + Symbol + "\");");
		}
		unsigned char byte2 = static_cast<unsigned char>(Symbol[1]);
		unsigned char byte3 = static_cast<unsigned char>(Symbol[2]);
		unsigned char byte4 = static_cast<unsigned char>(Symbol[3]);
		return ((byte1 & 0b00000111) << 18) | ((byte2 & 0b00111111) << 12) | ((byte3 & 0b00111111) << 6) | (byte4 & 0b00111111);
	}
	else {
		Error(Base_SA, "Cannot convert string-symbol '" + Symbol + "' to number because the symbol consists of two characters! ConvertStringSymbolToNumber(\"" + Symbol + "\");");
	}
	return -1;
}

/* Декодировать строку в Unicode */
std::vector<uint32_t> DecodeUTF8(const std::string& str) {
	std::vector<uint32_t> codePoints;
	size_t i = 0;

	while (i < str.size()) {
		uint32_t codePoint = 0;
		unsigned char byte = str[i];

		if ((byte & 0b10000000) == 0b00000000) {
			codePoint = byte;
			i += 1;
		}
		else if ((byte & 0b11100000) == 0b11000000) {
			if (i + 1 >= str.size()) {
				Error(Base_SA, "It is impossible to decode the string [" + std::to_string(str[i]) + "] because incomplete 2-byte sequence! DecodeUTF8(\"" + str + "\");");
			}
			codePoint = ((byte & 0b00011111) << 6) | (str[i + 1] & 0b00111111);
			i += 2;
		}
		else if ((byte & 0b11110000) == 0b11100000) {
			if (i + 2 >= str.size()) {
				Error(Base_SA, "It is impossible to decode the string [" + std::to_string(str[i]) + "] because incomplete 3-byte sequence! DecodeUTF8(\"" + str + "\");");
			}
			codePoint = ((byte & 0b00001111) << 12) | ((str[i + 1] & 0b00111111) << 6) | (str[i + 2] & 0b00111111);
			i += 3;
		}
		else if ((byte & 0b11111000) == 0b11110000) {
			if (i + 3 >= str.size()) {
				Error(Base_SA, "It is impossible to decode the string [" + std::to_string(str[i]) + "] because incomplete 4-byte sequence! DecodeUTF8(\"" + str + "\");");
			}
			codePoint = ((byte & 0b00000111) << 18) | ((str[i + 1] & 0b00111111) << 12) | ((str[i + 2] & 0b00111111) << 6) | (str[i + 3] & 0b00111111);
			i += 4;
		}
		else {
			Error(Base_SA, "It is impossible to decode the string [" + std::to_string(str[i]) + "] because it is of unknown byte pattern! DecodeUTF8(\"" + str + "\");");
		}

		codePoints.push_back(codePoint);
	}

	return codePoints;
}

/* В строке только ASCII символы */
bool OnlyASCIIChars(const std::string& str) {
	for (wchar_t ch : str) {
		if (ch > 127) {
			return false;
		}
	}
	return true;
}

/* ==== Конвертирование строк ==== */

std::wstring StringToWString(std::string str) {
	std::vector<uint32_t> codePoints = DecodeUTF8(str);

	std::wstring wideStr;
	for (uint32_t codePoint : codePoints) {
		wideStr += static_cast<wchar_t>(codePoint);
	}

	return wideStr;
}

/* ==== Конвертация в строки ==== */

std::string ToStringVec2(const glm::vec2 target) {
	return "glm::vec2(" + ToStringNumber(target.x) + "," + ToStringNumber(target.y) + ")";
}

std::string ToStringVec4(const glm::vec4 target) {
	return "glm::vec4(" + ToStringNumber(target.x) + "," + ToStringNumber(target.y) + "," + ToStringNumber(target.z) + "," + ToStringNumber(target.w) + ")";
}

std::string ToStringBool(const bool target) {
	return target ? "true" : "false";
}

std::string ToStringNumber(const double target) {
	std::ostringstream stream;
	stream << std::setprecision(15) << std::noshowpoint << target;
	return stream.str();
}