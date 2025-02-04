#include <string>

#include "BaseConstants.h";
#include "GameData.h";

/* 
 * Обозначения версии (x.y) 
 * x - Номер полной сборки     (публичный релиз               )
 * y - Номер отладочной сборки (точнее отправки коммита на гит)
 */

const int GameVersionMajor = 0;
const int GameVersionMinor = 1;
const int GameVersionPatch = 47;

std::string GetGameVersionPatch() {
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
    return std::to_string(GameVersionMinor) + "." + GetGameVersionPatch() + (DeveloperVersion ? " DEV" : "");
}