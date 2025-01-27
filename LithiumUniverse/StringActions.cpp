#include <GLM/glm.hpp>

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

/* Конвертировать строку u32string в string */
std::string u32stringToString(const std::u32string& u32str) {
	std::string str;
	for (char32_t ch : u32str) {
		if (ch <= 0x7F) {
			// 1 байт (ASCII)
			str.push_back(static_cast<char>(ch));
		}
		else if (ch <= 0x7FF) {
			// 2 байта
			str.push_back(static_cast<char>(0xC0 | (ch >> 6)));
			str.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
		}
		else if (ch <= 0xFFFF) {
			// 3 байта
			str.push_back(static_cast<char>(0xE0 | (ch >> 12)));
			str.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
			str.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
		}
		else if (ch <= 0x10FFFF) {
			// 4 байта
			str.push_back(static_cast<char>(0xF0 | (ch >> 18)));
			str.push_back(static_cast<char>(0x80 | ((ch >> 12) & 0x3F)));
			str.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
			str.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
		}
		else {
			Warn(Base_SA,"Failed to convert std::u32string to std::string! u32stringToString(?);");
			return "ERROR";
		}
	}
	return str;
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