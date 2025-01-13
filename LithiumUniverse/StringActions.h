#pragma once
#include <string>

extern const std::string Base;

std::string Trim(std::string Target);
std::string FillString(std::string Target, char Symbol, int Length, bool ToRight);
std::string ReplaceCharsToChars(std::string Target, char ToThis, char ToThat);
std::string ReplaceCharsToString(std::string Target, char ToThis, std::string ToThat);
char GetLastSymbol(std::string Target);
std::string RemoveLastSymbol(std::string Target);