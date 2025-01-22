#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <string>
#include <GLM/glm.hpp>

#include "GlobalResources.h";
#include "StringActions.h";
#include "GameResource.h";
#include "GameData.h";
#include "Texture.h";

/* Тип объекта */
enum RO_Type {
	RO_Default = 0, /* Обычный объект  */
	RO_UI      = 1, /* Интерфейс       */
	RO_Phys    = 2  /* Физичный объект */
};

/* Тип рендера объекта */
enum RenderTypeEnum {
	RT_Square  = 1, /* Квадрат */
	RT_Polygon = 0, /* Полигон */
	RT_Line    = 2, /* Линия   */
	RT_Text    = 3  /* Текст   */
};

/* Тип коллизии */
enum ColliderType {
	CT_None   = 0, /* Отсутвует */
	CT_Box    = 1, /* Прямоугольная */
	CT_Circle = 2, /* Круг */
	CT_Custom = 3  /* Кастомная */
};

/* Пустой объект, который можно рендерить */
class GameObject {
private:
	int ID = -1;
public:

	/* ==== Основные настройки ==== */
	std::string Name;          /* Название объекта  */
	RO_Type Type = RO_Default; /* Тип объекта       */
	bool Active = true;        /* Объект активен?   */
	bool Deleted = false;	   /* Объект удалённый? */
	bool DontDelete = false;   /* Не удалять объект */

	/* ==== Настройки рендера ==== */
	int BaseTextureRes           = 0;                      /* Базовая текстура                */
	int BaseShaderRes            = 0;                      /* Базовый шейдер                  */
	int FontRes                  = 0;                      /* Шрифт                           */
	glm::vec2 PositionVisual     = glm::vec2(0, 0);        /* Позиция объекта                 */
	glm::vec4 LinePositionVisual = glm::vec4(-1, 0, 1, 0); /* Позиция начала и конца линии    */
	glm::vec2 SizeVisual         = glm::vec2(1, 1);        /* Размер объекта                  */
	float OrientationVisual      = 0;                      /* Поворот объекта                 */
	glm::vec4 Color              = glm::vec4(1, 1, 1, 1);  /* Цвет объекта                    */
	float Layer                  = 0;                      /* Слой объекта                    */
	bool Render                  = true;                   /* Рендерить объект?               */
	RenderTypeEnum RenderType    = RT_Square;              /* Какие вертиксы рендерить?       */
	bool Selectable              = false;                  /* Мышка реагирует на этот объект? */

	/* ==== Настройки интерфейса ==== */
	bool Resize  = false;                  /* Менять размер вместе с экраном? */
	int ButtonID = -1;                     /* Является ли компонент кнопкой?  */
	std::string Text = "";                 /* Текст (если этот элемент текст) */

	/* ==== Настройки физики ==== */
	int BodyID             = -1;                     /* Айди b2BodyId                   */
	bool Static            = false;                  /* Объект статичный?               */
	ColliderType Collider  = CT_Box;                 /* Тип коллизии                    */

	GameObject(std::string Name_, int ID_) {
		ID = ID_;
		Name = Name_;
	}

	int GetID() const {
		return ID;
	}

	void Delete() {
		Deleted = true;
		PositionVisual = glm::vec2(0, 0);
		LinePositionVisual = glm::vec4(0, 0, 0, 0);
		SizeVisual = glm::vec2(0, 0);
		Color = glm::vec4(0, 0, 0, 0);
		Text = "";
	}

	std::string ToString() {
		return "GameObject(" + Name + "," + std::to_string(GetID()) + ");";
	}

	/* Установить ширину линии */
	void SetLineThickness(float th) {
		SizeVisual = glm::vec2(th,0);
	}

	/* Получить ширину линии */
	float GetLineThickness() {
		return SizeVisual.x;
	}

	/* Установить позицию линии */
	void SetLinePosition(glm::vec2 StartPos, glm::vec2 EndPos) {
		LinePositionVisual = glm::vec4(StartPos.x, StartPos.y, EndPos.x, EndPos.y);
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
		RenderType = RT_Line;
		//BaseShader = 2;

		SetLineThickness(th);
		SetLinePosition(StartPos, EndPos);
	}

	/* Получить стартовую позицию точки */
	glm::vec2 GetLineStartPosition() {
		return glm::vec2(LinePositionVisual.x, LinePositionVisual.y);
	}

	/* Получить конечную позицию точки */
	glm::vec2 GetLineEndPosition() {
		return glm::vec2(LinePositionVisual.z, LinePositionVisual.w);
	}
};

#endif