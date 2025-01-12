#include <algorithm>
#include <iostream>
#include <string>

#include "Console.h";

/* Скрипт позволяющий более углублённо управлять строками */

const std::string Base = "StringActions";

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
		Warn(Base, "Cannot delete the last character in a string because the string is empty! RemoveLastSymbol(\""+Target+"\");");
		return Target;
	}
	else {
		Target.pop_back();
		return Target;
	}
}