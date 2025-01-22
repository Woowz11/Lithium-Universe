#pragma once
#include <GLM/glm.hpp>
#include <string>

std::string ReplaceStringToString(const std::string Target, const std::string This, const std::string ToThat);
std::string Trim(const std::string& Target);
std::string FillString(const std::string&, const char Symbol, const int Length, const bool ToLeft);
std::string ReplaceCharsToChars(const std::string& Target, const char ToThis, const char ToThat);
std::string ReplaceCharsToString(const std::string& Target, const char ToThis, const std::string ToThat);
char GetLastSymbol(const std::string& Target);
std::string RemoveLastSymbol(const std::string& Target);
std::string RemoveFirstSymbol(const std::string& Target);
std::string u32stringToString(const std::u32string& u32str);

std::string ToStringBool(const bool target);
std::string ToStringVec2(const glm::vec2 target);
std::string ToStringVec4(const glm::vec4 target);