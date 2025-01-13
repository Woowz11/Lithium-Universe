#include <string>
#include <GLM/glm.hpp>

static int TotalIDs = 0;

enum ShapeType {
	Square = 1,  /* Квадрат */
	Polygon = 0  /* Полигон */
};

/* Пустой объект, который можно рендерить */
class RenderedObject {
private:
	int ID;

public:
	std::string Name;

	int BaseTexture = 0; /* Базовая текстура */
	int BaseShader  = 0; /* Базовый шейдер   */

	glm::vec2 Position    = glm::vec2(0, 0);       /* Позиция объекта                 */
	glm::vec2 Size        = glm::vec2(1, 1);       /* Размер объекта                  */
	glm::vec3 Orientation = glm::vec3(0, 0, 0);    /* Поворот объекта                 */
	glm::vec4 Color       = glm::vec4(1, 1, 1, 1); /* Цвет объекта                    */
	float Layer           = 0;                     /* Слой объекта                    */
	glm::mat4 Matrix      = glm::mat4(1);          /* Результативная позиция объекта  */
	bool Render           = true;                  /* Рендерить объект?               */
	ShapeType Shape       = Square;                /* Какие вертиксы рендерить?       */
	bool ThatUI           = false;                 /* Прикрепить объект к камере?     */
	bool Resize           = false;                 /* Менять размер вместе с экраном? */

	RenderedObject(std::string Name_) {
		TotalIDs++;
		ID = TotalIDs;
		Name = Name_;
	}

	int GetID() const {
		return ID;
	}
};