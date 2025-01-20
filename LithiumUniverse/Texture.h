#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>

class Texture
{
public:
	unsigned int ID       = -1;    /* Айди текстуы (если используется в атласе, то айди атласа) */
	int X                 = 0;     /* Позиция текстуры в атласе по X                            */
	int Y                 = 0;     /* Позиция текстуры в атласе по Y                            */
	int Width             = 1;     /* Ширина текстуры                                           */
	int Height            = 1;     /* Высота текстуры                                           */
	bool HasAlpha         = false; /* Есть альфа канал?                                         */
	int Atlas             = -1;    /* Айди атласа                                               */
	std::string AtlasName = "";    /* Название атласа                                           */
	std::string Path      = "";    /* Путь текстуры, так сказать название                       */
	std::string FullPath  = "";    /* Полный путь текстуры                                      */
	bool Blur             = false; /* Текстура размыленная?                                     */
	bool Error            = false; /* С ошибкой?                                                */

	Texture(std::string p, std::string a) {
		FullPath = p;
		AtlasName = a;
	}
};

#endif

extern int NotSelectedTexture;
extern std::vector<Texture> Texturies;
void DeleteTexture(Texture& T);
Texture CreateTexture(std::string Path, std::string Atlas);