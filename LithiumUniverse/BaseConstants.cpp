#include "BaseConstants.h";
#include <string>

const int GameVersionMajor = 0;
const int GameVersionMinor = 0;
const int GameVersionPatch = 5;

std::string GetGameVersionPatchWithLetters() {
    const std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    std::string result;

    int base = alphabet.size();
    int prefix = GameVersionPatch / base;
    int suffixIndex = GameVersionPatch % base;

    result += std::to_string(prefix);

    if (suffixIndex > 0) {
        result += alphabet[suffixIndex];
    }
    return result;
}

std::string GetGameVersion() {
    return std::to_string(GameVersionMajor) + "." + std::to_string(GameVersionMinor) + "." + GetGameVersionPatchWithLetters();
}