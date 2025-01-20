#ifndef GAME_RESOURCE_H
#define GAME_RESOURCE_H

#include <string>

enum GameResourceType {
	GR_Other   = 0,
	GR_Texture = 1,
	GR_Sound   = 2,
	GR_Shader  = 3,
	GR_Script  = 4
};

class GameResource {
public:
	std::string FullPath;  /* Путь до ресурса                 */
	GameResourceType Type; /* Тип ресурса                     */
	int ID;                /* Айди GameResource               */
	int AssetID;           /* Сам ресурс                      */
	bool Deleted = false;  /* Ресурс удалён?                  */

	GameResource(std::string FullPath_, GameResourceType Type_, int ID_, int AssetID_) {
		FullPath = FullPath_;
		Type = Type_;
		ID = ID_;
		AssetID = AssetID_;
	}
};

#endif