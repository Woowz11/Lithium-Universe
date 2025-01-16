#include "BaseConstants.h";
#include <string>

/* Обозначения версии (x.y.z) 
 * x - Номер полной сборки     (публичный релиз)
 * y - Номер тестовой сборки   (частично играбельное, для теста, друзьям)
 * z - Номер отладочной сборки (точнее отправки коммита на гит)
 */
const int GameVersionMajor = 0;
const int GameVersionMinor = 1;
const int GameVersionPatch = 37;

std::string GetGameVersionPatchWithLetters() {
    const std::string alphabet = "ABCDEFGHIJKLMNoPQRSTUVWXYZ";
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