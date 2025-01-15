#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <string>
#include <GLM/glm.hpp>

#include "StringActions.h";
#include "StaticInfo.h";
#include "Collider.h";
#include "Console.h";

/* Тип объекта */
enum RO_Type {
	RO_Default = 0, /* Обычный объект  */
	RO_UI      = 1, /* Интерфейс       */
	RO_Phys    = 2  /* Физичный объект */
};

/* Тип рендера объекта */
enum ShapeType {
	ST_Square  = 1, /* Квадрат */
	ST_Polygon = 0, /* Полигон */
	ST_Line    = 2  /* Линия   */
};

/* Пустой объект, который можно рендерить */
class GameObject {
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

	int BaseTexture = 1; /* Базовая текстура */
	int BaseShader  = 1; /* Базовый шейдер   */

	glm::vec2 Position     = glm::vec2(0, 0);        /* Позиция объекта                 */
	glm::vec4 LinePosition = glm::vec4(-1, 0, 1, 0); /* Позиция начала и конца линии    */
	glm::vec2 Size         = glm::vec2(1, 1);        /* Размер объекта                  */
	float Orientation      = 0;                      /* Поворот объекта                 */
	glm::vec4 Color        = glm::vec4(1, 1, 1, 1);  /* Цвет объекта                    */
	float Layer            = 0;                      /* Слой объекта                    */
	bool Render            = true;                   /* Рендерить объект?               */
	ShapeType Shape        = ST_Square;              /* Какие вертиксы рендерить?       */
	bool ThatUI            = false;                  /* Прикрепить объект к камере?     */
	bool Resize            = false;                  /* Менять размер вместе с экраном? */
	Collider Col           = Collider(CLDR_None);    /* Коллизия                        */
	bool Selectable        = false;                  /* Мышка реагирует на этот объект? */

	/* ==== Физические параметры (большая часть из них работает только с RO_Phys) ==== */

	bool Static           = false;           /* Статичный?        */
	glm::vec2 Velocity    = glm::vec2(0, 0); /* Скорость          */
	float Mass            = 1;               /* Вес объекта       */
	float Restitution     = 0;               /* Упругость         */
	float AngularVelocity = 0;               /* Скорость вращения */

	GameObject(std::string Name_, RO_Type type) {
		ID = TotalIDs++;
		Name = Name_;
		Type = type;

		switch (type)
		{
		case RO_UI:
			Resize = true;
			ThatUI = true;
			Selectable = true;
			Layer  = 500;
			break;
		case RO_Phys:
			Col = Collider(CLDR_Custom);
			Selectable = true;
			break;
		default:
			break;
		}
	}
	GameObject(std::string Name_) : GameObject(Name_, RO_Default) {}

	int GetID() const {
		return ID;
	}

	/* Применить скорость */
	void Impulse(glm::vec2 Vel) {
		if (!Static) {
			Velocity += Vel;
		}
	}

	/* Получить скорость */
	glm::vec2 GetVelocity() {
		return Static ? glm::vec2(0,0) : Velocity;
	}

	/* Получить физическую позицию, она измеряется с верхнего левого угла, а не с центра фигуры */
	glm::vec2 GetPhysicalPosition() {
		return Position - glm::vec2(Size.x/2, Size.y/2);
	}

	/* Получить размер объекта */
	float GetSize() {
		return Size.x * Size.y;
	}

	/* Влияние формы объекта на воздух */
	float AirResistanceDependentOnForm() {
		float Result = 0;
		switch (Col.Form)
		{
			case OF_Cube:
				Result = 1;
				break;
			case OF_Sphere:
				Result = 0.9f;
				break;
			case OF_Bullet:
				Result = 0.1f;
				break;
			case OF_Stick:
				Result = 0.01f;
				break;
			default:
				break;
		}

		return Result * GetSize();
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

	/* Установить ширину линии */
	void SetLineThickness(float th) {
		Size = glm::vec2(th,0);
	}

	/* Получить ширину линии */
	float GetLineThickness() {
		return Size.x;
	}

	/* Установить позицию линии */
	void SetLinePosition(glm::vec2 StartPos, glm::vec2 EndPos) {
		LinePosition = glm::vec4(StartPos.x, StartPos.y, EndPos.x, EndPos.y);
	}

	/* Установить позицию началу линии */
	void SetLineStartPosition(glm::vec2 Pos) {
		SetLinePosition(Pos, GetLineEndPosition());
	}

	/* Установить позицию концу линии */
	void SetLineEndPosition(glm::vec2 Pos) {
		SetLinePosition(GetLineStartPosition(), Pos);
	}

	/* Сделать линией */
	void MakeItLine(glm::vec2 StartPos, glm::vec2 EndPos, float th) {
		Shape = ST_Line;
		BaseShader = 2;

		SetLineThickness(th);
		SetLinePosition(StartPos, EndPos);
	}

	/* Получить стартовую позицию точки */
	glm::vec2 GetLineStartPosition() {
		return glm::vec2(LinePosition.x, LinePosition.y);
	}

	/* Получить конечную позицию точки */
	glm::vec2 GetLineEndPosition() {
		return glm::vec2(LinePosition.z, LinePosition.w);
	}
};

#endif