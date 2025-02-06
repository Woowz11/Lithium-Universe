#pragma once
#include <GLM/glm.hpp>
#include <vector>
#include <string>

std::string ReplaceStringToString(const std::string Target, const std::string This, const std::string ToThat);
std::string Trim(const std::string& Target);
std::string FillString(const std::string&, const char Symbol, const int Length, const bool ToLeft);
std::string RemoveStringPart(const std::string& Target, const int Length);
std::string ReplaceCharsToChars(const std::string& Target, const char ToThis, const char ToThat);
std::string ReplaceCharsToString(const std::string& Target, const char ToThis, const std::string ToThat);
bool StringStartWith(const std::string& Target, const std::string StartWith);
int GetLinesCountFromString(const std::string& Target);
std::string GetLineFromString(const std::string& Target, const int SelectedLine);
char GetLastSymbol(const std::string& Target);
std::string RepeatString(const std::string& Target, const int CoutRepeat);
std::string RemoveLastSymbol(const std::string& Target);
std::string RemoveFirstSymbol(const std::string& Target);
std::vector<int> StringToNumbers(const std::string Target);
int ConvertStringSymbolToNumber(const std::string Symbol);
std::vector<uint32_t> DecodeUTF8(const std::string& str);
bool OnlyASCIIChars(const std::string& str);

std::wstring StringToWString(std::string str);

std::string ToStringBool(const bool target);
std::string ToStringVec2(const glm::vec2 target);
std::string ToStringVec4(const glm::vec4 target);
std::string ToStringNumber(const double target);