#pragma once
#include <GLM/glm.hpp>
#include <string>

std::string Trim(std::string Target);
std::string FillString(std::string Target, char Symbol, int Length, bool ToRight);
std::string ReplaceCharsToChars(std::string Target, char ToThis, char ToThat);
std::string ReplaceCharsToString(std::string Target, char ToThis, std::string ToThat);
char GetLastSymbol(std::string Target);
std::string RemoveLastSymbol(std::string Target);

std::string ToStringBool(bool target);
std::string ToStringVec2(glm::vec2 target);
std::string ToStringVec4(glm::vec4 target);