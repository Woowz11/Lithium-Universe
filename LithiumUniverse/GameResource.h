#ifndef GAME_RESOURCE_H
#define GAME_RESOURCE_H

#include <string>

enum GameResourceType {
	GR_Texture,
	GR_Sound,
	GR_Shader,
	GR_Script,
	GR_Other
};

class GameResource {
public:
	std::string FullPath;
	std::string Base;      /* Модификация (основа ресурса)    */
	std::string Path;      /* Путь до ресурса (в модификации) */
	GameResourceType Type; /* Тип ресурса                     */
	int ID;                /* Айди GameResource               */
	int AssetID;           /* Сам ресурс                      */
	bool Deleted = false;  /* Ресурс удалён?                  */

	GameResource(std::string FullPath_ ,std::string Base_, std::string Path_, GameResourceType Type_, int ID_, int AssetID_) {
		FullPath = FullPath_;
		Base = Base_;
		Path = Path_;
		Type = Type_;
		ID = ID_;
		AssetID = AssetID_;
	}

	int Get() {
		if (Deleted) {
			return 0;
		}
		else {
			return AssetID;
		}
	}
};

#endif