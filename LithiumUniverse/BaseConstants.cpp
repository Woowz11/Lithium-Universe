#include "BaseConstants.h";
#include <string>

/* Обозначения версии (x.y.z) 
 * x - Номер полной сборки     (публичный релиз)
 * y - Номер душевной сборки   (когда сделалось что-то глобальное, то +1)
 * z - Номер отладочной сборки (точнее отправки коммита на гит)
 */
const int GameVersionMajor = 0;
const int GameVersionMinor = 1;
const int GameVersionPatch = 40;

std::string GetGameVersionPatchWithLetters() {
    const std::string alphabet = "aAbBcCdDxXyYzZwW";
    std::string result;

    int base = static_cast<int>(alphabet.size());
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