#ifndef RENDERED_OBJECT_H
#define RENDERED_OBJECT_H

#include <string>
#include <GLM/glm.hpp>

#include "StaticInfo.h";
#include "Collider.h";
#include "Console.h";

enum RO_Type {
	RO_Default,
	RO_UI,
	RO_Phys
};

enum ShapeType {
	ST_Square = 1,  /* Квадрат */
	ST_Polygon = 0  /* Полигон */
};

/* Пустой объект, который можно рендерить */
class RenderedObject {
private:
	int ID;

	void CheckOrientationLimits() {
		float z = Orientation;
		if (z > 360) {
			z = 0;
		}
		if (z < 0) {
			z = 360;
		}
		Orientation = z;
	}

public:
	std::string Name;

	RO_Type Type; /* Тип объекта */

	bool Active = true; /* Объект активен? */

	int BaseTexture = 0; /* Базовая текстура */
	int BaseShader  = 0; /* Базовый шейдер   */

	glm::vec2 Position    = glm::vec2(0, 0);       /* Позиция объекта                 */
	glm::vec2 Size        = glm::vec2(1, 1);       /* Размер объекта                  */
	float Orientation     = 0;                     /* Поворот объекта                 */
	glm::vec4 Color       = glm::vec4(1, 1, 1, 1); /* Цвет объекта                    */
	float Layer           = 0;                     /* Слой объекта                    */
	bool Render           = true;                  /* Рендерить объект?               */
	ShapeType Shape       = ST_Square;             /* Какие вертиксы рендерить?       */
	bool ThatUI           = false;                 /* Прикрепить объект к камере?     */
	bool Resize           = false;                 /* Менять размер вместе с экраном? */
	Collider Col          = Collider(CLDR_None);   /* Коллизия */

	RenderedObject(std::string Name_, RO_Type type) {
		TotalIDs++;
		ID = TotalIDs;
		Name = Name_;
		Type = type;

		switch (type)
		{
		case RO_UI:
			Resize = true;
			ThatUI = true;
			Layer  = 500;
			break;
		case RO_Phys:
			Col = CLDR_Square;
			break;
		default:
			break;
		}
	}
	RenderedObject(std::string Name_) : RenderedObject(Name_, RO_Default) {}

	int GetID() const {
		return ID;
	}

	/* Получить физическую позицию, она измеряется с верхнего левого угла, а не с центра фигуры */
	glm::vec2 GetPhysicalPosition() {
		return Position - glm::vec2(Size.x/2, Size.y/2);
	}

	/* Установить позицию объекту */
	void SetPosition(float x, float y) {
		Position = glm::vec2(x, y);
	}
	void SetPosition(glm::vec2 pos) {
		Position = pos;
	}

	/* Добавить позицию объекту */
	void AddPosition(float x, float y) {
		Position += glm::vec2(x, y);
	}

	/* Повернуть объект */
	void SetRotation(float deg) {
		Orientation = deg;
		CheckOrientationLimits();
	}

	/* Постепенно поварачивать объект */
	void AddRotation(float deg) {
		Orientation += deg;
		CheckOrientationLimits();
	}
};

#endif