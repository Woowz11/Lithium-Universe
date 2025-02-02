#include <string>

#include "BaseConstants.h";
#include "GameData.h";

/* 
 * Обозначения версии (x.y) 
 * x - Номер полной сборки     (публичный релиз               )
 * y - Номер отладочной сборки (точнее отправки коммита на гит)
 */

const int GameVersionMajor = 0;
const int GameVersionMinor = 45;

std::string GetGameVersionMinor() {
    const std::string alphabet = "aAbBcCdDxXyYzZwW";
    std::string result;

    int base = static_cast<int>(alphabet.size());
    int prefix = GameVersionMinor / base;
    int suffixIndex = GameVersionMinor % base;

    result += std::to_string(prefix);

    if (suffixIndex > 0) {
        result += alphabet[suffixIndex];
    }
    return result;
}

std::string GetGameVersion() {
    return std::to_string(GameVersionMajor) + "." + GetGameVersionMinor() + (DeveloperVersion ? " DEV" : "");
}