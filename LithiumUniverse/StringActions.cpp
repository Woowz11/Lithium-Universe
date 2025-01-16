#include <GLM/glm.hpp>

#include <algorithm>
#include <iostream>
#include <string>

#include "GameObject.h";
#include "Console.h";

/* Скрипт позволяющий более углублённо управлять строками */

const std::string Base_SA = "StringActions";

/* Убрать пробелы из начала и конца */
std::string Trim(std::string Target) {
    size_t first = Target.find_first_not_of(" \t\n\r");
    size_t last = Target.find_last_not_of(" \t\n\r");

    if (first == std::string::npos || last == std::string::npos) {
        return "";
    }

    return Target.substr(first, last - first + 1);
}

/* Заполнить строку символами */
std::string FillString(std::string Target, char Symbol, int Length, bool ToRight) {
	if (Target.length() >= Length) {
		return Target;
	}

	int fillLength = Length - Target.length();
	std::string filledString(fillLength, Symbol);
	return ToRight ? filledString + Target : Target + filledString;
}

/* Заменить все символы в строке на другие символы */
std::string ReplaceCharsToChars(std::string Target, char ToThis, char ToThat) {
	std::string Result = Target;
	std::replace(Result.begin(), Result.end(), ToThis, ToThat);
	return Result;
}

/* Заменить все символы в строке на строку */
std::string ReplaceCharsToString(std::string Target, char ToThis, std::string ToThat) {
	std::string Result = Target;
	std::string Char(1, ToThis);
	size_t pos = 0;
	while ((pos = Result.find(Char, pos)) != std::string::npos) {
		Result.replace(pos, Char.length(), ToThat);
		pos += ToThat.length();
	}
	return Result;
}

/* Получить последний символ строки */
char GetLastSymbol(std::string Target) {
	if (Target.empty()) {
		return ' ';
	}
	return Target.back();
}

/* Удалить последний символ строки */
std::string RemoveLastSymbol(std::string Target) {
	if (Target.empty()) {
		Warn(Base_SA, "Cannot delete the last character in a string because the string is empty! RemoveLastSymbol(\""+Target+"\");");
		return Target;
	}
	else {
		Target.pop_back();
		return Target;
	}
}

/* ==== Конвертация в строки ==== */

std::string ToStringVec2(glm::vec2 target) {
	return "glm::vec2(" + std::to_string(target.x) + "," + std::to_string(target.y) + ")";
}

std::string ToStringVec4(glm::vec4 target) {
	return "glm::vec4(" + std::to_string(target.x) + "," + std::to_string(target.y) + "," + std::to_string(target.z) + "," + std::to_string(target.w) + ")";
}

std::string ToStringBool(bool target) {
	return target ? "true" : "false";
}