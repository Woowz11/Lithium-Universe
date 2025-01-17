#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>

#include <string>
#include <vector>
#include <box2cpp/box2cpp.h>

#include "StringActions.h";
#include "GameObject.h";
#include "GameData.h";
#include "Console.h";

GameObject ErrorGameObject = GameObject("ERROR", -1);

/* ==== Сцена ==== */

/* Сцена */
std::vector<GameObject> Scene = {};

/* Физичные объекты */
std::vector<b2::Body>    Bodies    = {};
std::vector<b2::BodyRef> BodiesRef = {};

/* Обновить коллизию объекта (private) */
void RefreshGameObjectCollider__(GameObject& OBJ) {
	switch (OBJ.Collider)
	{
	case CT_Box:
		Bodies[OBJ.BodyID].CreateShape(
			b2::DestroyWithParent,
			b2::Shape::Params{},
			b2MakeBox(OBJ.SizeVisual.x, OBJ.SizeVisual.y)
		);
		break;
	case CT_Circle:
		Bodies[OBJ.BodyID].CreateShape(
			b2::DestroyWithParent,
			b2::Shape::Params{},
			b2Circle(b2Vec2(0,0), OBJ.SizeVisual.x)
		);
		break;
	default:
		Bodies[OBJ.BodyID].CreateShape(
			b2::DestroyWithParent,
			b2::Shape::Params{},
			b2MakeBox(OBJ.SizeVisual.x, OBJ.SizeVisual.y)
		);
		break;
	}
}

glm::vec2 BVec2ToVec2(b2Vec2 v) {
	return glm::vec2(v.x, v.y);
}
b2Vec2 Vec2ToBVec2(glm::vec2 v) {
	return b2Vec2(v.x, v.y);
}
b2Rot Makeb2Rot(float o) {
	return b2Rot(cosf(-o), sinf(o));
}
float MakeOrientation(b2Rot r) {
	return -atan2(r.s, r.c);
}

b2::Body& GetBody(int id) {
	return Bodies[id];
}

/* Получить объект через физический объект */
GameObject& GetGameObjectFromBody(b2::Body b) {
	auto it = std::find_if(Bodies.begin(), Bodies.end(),
		[&b](const b2::Body& body) { return &body == &b; });
	if (it != Bodies.end()) {
		return Scene[std::distance(Bodies.begin(), it)];
	}
	else {
		Error("GameObject", "GameObject not found via b2::Body! GetGameObjectFromBody(?);");
		return ErrorGameObject;
	}
}
GameObject& GetGameObjectFromBodyRef(b2::BodyRef b) {
	auto it = std::find_if(BodiesRef.begin(), BodiesRef.end(),
		[&b](const b2::BodyRef& body) { return &body == &b; }); /* КОРОЧЕ, Я СПАТЬ, СМОТРИ, УДАЛЯЙ НАХУЙ BOX2DCPP, ДЕЛАЙ НА ЧИСТОМ BOX2D */
	if (it != BodiesRef.end()) {
		return Scene[std::distance(BodiesRef.begin(), it)];
	}
	else {
		Error("GameObject","GameObject not found via b2::BodyRef! GetGameObjectFromBodyRef(?);");
		return ErrorGameObject;
	}
}

/* Предупредить, что объект не подходит для физики (private) */
void GameObjectNotSuitableForPhysics__(GameObject& OBJ, std::string Func) {
	WarnSerious("GAMEOBJECT", "It is impossible to perform function " + Func + " because " + OBJ.ToString() + " is not for physics!");
}

/* Предупредить, что объект не подходит для всего, потому-что удалён (private) */
void GameObjectDeleted__(GameObject& OBJ, std::string Func) {
	WarnSerious("GAMEOBJECT", "It is impossible to perform function " + Func + " because " + OBJ.ToString() + " has been deleted!");
}

/* Сделать объект физичным (private) */
void MakeGameObjectPhysical__(GameObject& OBJ) {
	OBJ.Type = RO_Phys;
	OBJ.Selectable = true;

	int BodyID = Bodies.size();
	OBJ.BodyID = BodyID;

	b2::Body::Params P;
	P.type = b2_dynamicBody;

	Bodies.push_back(World->CreateBody(b2::OwningHandle, P));
	BodiesRef.push_back(Bodies[BodyID]);
	RefreshGameObjectCollider__(OBJ);
}

/* Получить цвет объекта */
glm::vec4 GetGameObjectColor(int i) {
	return Scene[i].Color;
}

/* Установить цвет объекту */
void SetGameObjectColor(int i, glm::vec4 c) {
	GameObject& OBJ = Scene[i];
	if (!OBJ.Deleted) {
		OBJ.Color = c;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectSize(" + std::to_string(i) + "," + ToStringVec4(c) + ");");
	}
}

/* Получить размер объекта */
glm::vec2 GetGameObjectSize(int i) {
	return Scene[i].SizeVisual;
}

/* Установить размер объекту */
void SetGameObjectSize(int i, glm::vec2 s) {
	GameObject& OBJ = Scene[i];
	if (!OBJ.Deleted) {
		OBJ.SizeVisual = s;
		if (OBJ.Type == RO_Phys) {
			RefreshGameObjectCollider__(OBJ);
		}
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectSize(" + std::to_string(i) + "," + ToStringVec2(s) + ");");
	}
}

/* Установить, рендерить ли объект? */
void SetGameObjectRenderable(int i, bool b) {
	Scene[i].Render = b;
}

/* Получить позицию объекта */
glm::vec2 GetGameObjectPosition(int i) {
	return Scene[i].PositionVisual;
}

/* Получить поворот объекта */
float GetGameObjectOrientation(int i) {
	return Scene[i].OrientationVisual;
}

/* Установить позицию объекту */
void SetGameObjectPosition(int i, glm::vec2 p) {
	GameObject& OBJ = Scene[i];
	if (!OBJ.Deleted) {
		OBJ.PositionVisual = p;
		if (OBJ.Type == RO_Phys) {
			Bodies[OBJ.BodyID].SetTransform(Vec2ToBVec2(p), Makeb2Rot(GetGameObjectOrientation(i)));
		}
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectPosition(" + std::to_string(i) + "," + ToStringVec2(p) + ");");
	}
}

/* Установить поворот объекту */
void SetGameObjectOrientation(int i, float r) {
	GameObject& OBJ = Scene[i];
	if (!OBJ.Deleted) {
		OBJ.OrientationVisual = r;
		if (OBJ.Type == RO_Phys) {
			Bodies[OBJ.BodyID].SetTransform(Vec2ToBVec2(GetGameObjectPosition(i)), Makeb2Rot(r));
		}
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectOrientation(" + std::to_string(i) + "," + std::to_string(r) + ");");
	}
}

/* Установить позицию и поворот объекту */
void SetGameObjectTransform(int i, glm::vec2 p, float r) {
	GameObject& OBJ = Scene[i];
	if (!OBJ.Deleted) {
		OBJ.PositionVisual = p;
		OBJ.OrientationVisual = r;
		if (OBJ.Type == RO_Phys) {
			Bodies[OBJ.BodyID].SetTransform(Vec2ToBVec2(p), Makeb2Rot(r));
		}
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectPosition(" + std::to_string(i) + "," + ToStringVec2(p) + "," + std::to_string(r) + ");");
	}
}

/* Сделать объект статичным? */
void SetGameObjectStatic(int i, bool b) {
	GameObject& OBJ = Scene[i];
	if (!OBJ.Deleted && OBJ.Type == RO_Phys) {
		OBJ.Static = b;
		Bodies[OBJ.BodyID].SetType(b? b2_staticBody : b2_dynamicBody);
	}
	else {
		GameObjectNotSuitableForPhysics__(OBJ, "SetGameObjectStatic(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	}
}

/* Создать объект */
int CreateGameObject(std::string Name = "[New GameObject]", bool Physic = false) {
	GameObject OBJ = GameObject(Name, Scene.size());
	OBJ.BaseShader = 1;
	OBJ.BaseTexture = 1;
	if (Physic) {
		MakeGameObjectPhysical__(OBJ);
	}
	Scene.push_back(OBJ);
	return OBJ.GetID();
}