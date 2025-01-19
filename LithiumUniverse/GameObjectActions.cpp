#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>

#include <string>
#include <vector>
#include <box2d/box2d.h>

#include "StringActions.h";
#include "GlobalPhysic.h";
#include "GameObject.h";
#include "GameData.h";
#include "GlobalUI.h";
#include "Console.h";

GameObject ErrorGameObject = GameObject("ERROR", -1);

/* Слои */
enum Filteres
{
	F_None     = 0,
	F_All      = (~0u),
	F_World    = 0x00000001,
	F_Default  = 0x00000002,
	F_Disabled = 0x00000004
};

/* ==== Сцена ==== */

/* Сцена */
std::vector<GameObject> Scene = {};

/* Физичные объекты */
std::vector<b2BodyId> Bodies = {};

/* Удалить коллизию объекта (private) */
void RemoveGameObjectCollider__(GameObject& OBJ) {
	b2BodyId Body = Bodies[OBJ.BodyID];
	int i = b2Body_GetShapeCount(Body);
	b2ShapeId* shapes = new b2ShapeId[i];
	b2Body_GetShapes(Body, shapes, i);
	for (int j = 0; j < i; ++j) {
		b2ShapeId s = shapes[j];
		b2DestroyShape(s, true);
	}
	delete[] shapes;
}

/* Обновить коллизию объекта (private) */
void RefreshGameObjectCollider__(GameObject& OBJ) {
	RemoveGameObjectCollider__(OBJ);

	b2ShapeDef ShapeInfo = b2DefaultShapeDef();
	/* Чем сам по себе является? */
	ShapeInfo.filter.categoryBits = OBJ.Collider == CT_None ? F_Disabled : (OBJ.Static ? F_World : F_Default);
	/* С кем коллайдится */
	ShapeInfo.filter.maskBits     = OBJ.Collider == CT_None ? F_None : OBJ.Static ? F_World | F_Default : F_World | F_Default;

	switch (OBJ.Collider)
	{
	case CT_Box:
		b2Polygon ColBox = b2MakeBox(OBJ.SizeVisual.x, OBJ.SizeVisual.y);
		b2CreatePolygonShape(Bodies[OBJ.BodyID],
			&ShapeInfo,
			&ColBox
		);
		break;
	case CT_Circle:
		
		break;
	default:
		b2Polygon ColNone = b2MakeBox(0.01f, 0.01f);
		b2CreatePolygonShape(Bodies[OBJ.BodyID],
			&ShapeInfo,
			&ColNone
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

b2BodyId GetBody(int i) {
#ifdef NDEBUG
	return Bodies[i];
#else
	if (i >= 0 && i <= Bodies.size()) {
		return Bodies[i];
	}
	else {
		Error("GAMEOBJ/BODY", "It is not possible to get the b2BodyId because it cannot be found by ID in the bodies! GetBody(" + std::to_string(i) + ");");
		return { 0 };
	}
#endif
}
GameObject& GetGameObject(int i, std::string message) {
#ifdef NDEBUG
	return Scene[i];
#else
	if (i >= 0 && i <= Scene.size()) {
		return Scene[i];
	}
	else {
		Error("GAMEOBJ", "It is not possible to get the GameObject because it cannot be found by ID in the scene! GetGameObject(" + std::to_string(i) + "); + " + message);
		return ErrorGameObject;
	}
#endif
}

/* Получить объект через физический объект */
GameObject& GetGameObjectFromBody(b2BodyId b) {
	auto it = std::find_if(Bodies.begin(), Bodies.end(),
		[&b](const b2BodyId& body) { return body.index1 == b.index1; });
	if (it != Bodies.end()) {
		return GetGameObject(std::distance(Bodies.begin(), it), "GetGameObjectFromBody(?);");
	}
	else {
		Error("GAMEOBJ", "GameObject not found via b2BodyId! GetGameObjectFromBody(?);");
		return ErrorGameObject;
	}
}

/* Предупредить, что объект не подходит для физики (private) */
void GameObjectNotSuitableForPhysics__(GameObject& OBJ, std::string Func) {
	WarnSerious("GAMEOBJ", "It is impossible to perform function " + Func + " because " + OBJ.ToString() + " is not for physics!");
}

/* Предупредить, что объект не подходит для всего, потому-что удалён (private) */
void GameObjectDeleted__(GameObject& OBJ, std::string Func) {
	WarnSerious("GAMEOBJ", "It is impossible to perform function " + Func + " because " + OBJ.ToString() + " has been deleted!");
}

/* Сделать объект физичным (private) */
void MakeGameObjectPhysical__(int i) {
	GameObject& OBJ = GetGameObject(i, "MakeGameObjectPhysical__(" + std::to_string(i) + ");");
	OBJ.Type = RO_Phys;
	OBJ.Selectable = true;

	int BodyID = Bodies.size();
	OBJ.BodyID = BodyID;

	b2BodyDef BodyInfo = b2DefaultBodyDef();
	BodyInfo.type = b2_dynamicBody;
	b2BodyId Body = b2CreateBody(World, &BodyInfo);

	Bodies.push_back(Body);
	RefreshGameObjectCollider__(OBJ);
}

/* Получить игровой объект находящийся на этой точке */
int GetGameObjectFromPoint(glm::vec2 PointPos) {
	float PointSize = 0.00001f;
	b2Vec2 v1 = Vec2ToBVec2(PointPos);
	b2Vec2 v2 = b2Vec2(PointSize, PointSize);
	b2AABB MouseDetector(b2Sub(v1,v2), b2Add(v1,v2));

	int result = -1;

	b2World_OverlapAABB(World, MouseDetector, b2DefaultQueryFilter(), [](b2ShapeId shapeId, void* context) {
		b2BodyId bodyId = b2Shape_GetBody(shapeId);
		b2Body_SetAwake(bodyId, true);
		GameObject& OBJ = GetGameObjectFromBody(bodyId);
		
		if (OBJ.Selectable) {
			int* resultPtr = static_cast<int*>(context);

			*resultPtr = OBJ.GetID();

			return true;
		}
		else {
			return false;
		}
	}, &result);

	return result;
}

/* Установить, менять ли размер объекту взависимости от размера экрана? */
void SetGameObjectResize(int i, bool b) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectResize(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	if (!OBJ.Deleted) {
		OBJ.Resize = b;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectResize(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	}
}

/* Установить визуальный слой объекта */
void SetGameObjectLayer(int i, float l) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectLayer(" + std::to_string(i) + "," + std::to_string(l) + ");");
	if (!OBJ.Deleted) {
		OBJ.Layer = l;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectLayer(" + std::to_string(i) + "," + std::to_string(l) + ");");
	}
}

/* Установить текстуру объекту */
void SetGameObjectTexture(int i, int t) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectTexture(" + std::to_string(i) + "," + std::to_string(t) + ");");
	if (!OBJ.Deleted) {
		OBJ.BaseTexture = t;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectTexture(" + std::to_string(i) + "," + std::to_string(t) + ");");
	}
}

/* Установить, ативный ли объект? */
void SetGameObjectActive(int i, bool b) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectActive(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	if (!OBJ.Deleted) {
		OBJ.Active = b;
		if (OBJ.Type == RO_Phys) {
			if (b) {
				b2Body_Enable(Bodies[OBJ.BodyID]);
			}
			else {
				b2Body_Disable(Bodies[OBJ.BodyID]);
			}
		}
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectActive(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	}
}

/* Установить тип коллизии */
void SetGameObjectCollider(int i, ColliderType CT) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectCollider(" + std::to_string(i) + "," + std::to_string(CT) + ");");
	if (!OBJ.Deleted && OBJ.Type == RO_Phys) {
		OBJ.Collider = CT;
		RefreshGameObjectCollider__(OBJ);
	}
	else {
		GameObjectNotSuitableForPhysics__(OBJ, "SetGameObjectCollider(" + std::to_string(i) + "," + std::to_string(CT) + ");");
	}
}

/* Сделать объект выделяемым */
void SetGameObjectSelectable(int i, bool b) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectSelectable(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	if (!OBJ.Deleted) {
		OBJ.Selectable = b;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectSelectable(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	}
}

/* Получить цвет объекта */
glm::vec4 GetGameObjectColor(int i) {
	return GetGameObject(i, "GetGameObjectColor(" + std::to_string(i) + ");").Color;
}

/* Установить цвет объекту */
void SetGameObjectColor(int i, glm::vec4 c) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectSize(" + std::to_string(i) + "," + ToStringVec4(c) + ");");
	if (!OBJ.Deleted) {
		OBJ.Color = c;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectSize(" + std::to_string(i) + "," + ToStringVec4(c) + ");");
	}
}

/* Получить размер объекта */
glm::vec2 GetGameObjectSize(int i) {
	return GetGameObject(i, "GetGameObjectSize(" + std::to_string(i) + ");").SizeVisual;
}

/* Установить размер объекту */
void SetGameObjectSize(int i, glm::vec2 s) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectSize(" + std::to_string(i) + "," + ToStringVec2(s) + ");");
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
	GetGameObject(i, "SetGameObjectRenderable(" + std::to_string(i) + "," + ToStringBool(b) + ");").Render = b;
}

/* Получить позицию объекта */
glm::vec2 GetGameObjectPosition(int i) {
	return GetGameObject(i, "GetGameObjectPosition(" + std::to_string(i) + ");").PositionVisual;
}

/* Получить поворот объекта */
float GetGameObjectOrientation(int i) {
	return GetGameObject(i, "GetGameObjectOrientation(" + std::to_string(i) + ");").OrientationVisual;
}

/* Установить позицию объекту */
void SetGameObjectPosition(int i, glm::vec2 p) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectPosition(" + std::to_string(i) + "," + ToStringVec2(p) + ");");
	if (!OBJ.Deleted) {
		OBJ.PositionVisual = p;
		if (OBJ.Type == RO_Phys) {
			b2Body_SetTransform(Bodies[OBJ.BodyID], Vec2ToBVec2(p), Makeb2Rot(GetGameObjectOrientation(i)));
		}
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectPosition(" + std::to_string(i) + "," + ToStringVec2(p) + ");");
	}
}

/* Установить поворот объекту */
void SetGameObjectOrientation(int i, float r) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectOrientation(" + std::to_string(i) + "," + std::to_string(r) + ");");
	if (!OBJ.Deleted) {
		OBJ.OrientationVisual = r;
		if (OBJ.Type == RO_Phys) {
			b2Body_SetTransform(Bodies[OBJ.BodyID], Vec2ToBVec2(GetGameObjectPosition(i)), Makeb2Rot(r));
		}
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectOrientation(" + std::to_string(i) + "," + std::to_string(r) + ");");
	}
}

/* Установить позицию и поворот объекту */
void SetGameObjectTransform(int i, glm::vec2 p, float r) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectPosition(" + std::to_string(i) + "," + ToStringVec2(p) + "," + std::to_string(r) + ");");
	if (!OBJ.Deleted) {
		OBJ.PositionVisual = p;
		OBJ.OrientationVisual = r;
		if (OBJ.Type == RO_Phys) {
			b2Body_SetTransform(Bodies[OBJ.BodyID], Vec2ToBVec2(p), Makeb2Rot(r));
		}
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectPosition(" + std::to_string(i) + "," + ToStringVec2(p) + "," + std::to_string(r) + ");");
	}
}

/* Сделать объект статичным? */
void SetGameObjectStatic(int i, bool b) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectStatic(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	if (!OBJ.Deleted && OBJ.Type == RO_Phys) {
		OBJ.Static = b;
		b2Body_SetType(Bodies[OBJ.BodyID], b ? b2_staticBody : b2_dynamicBody);
	}
	else {
		GameObjectNotSuitableForPhysics__(OBJ, "SetGameObjectStatic(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	}
}

/* Создать объект */
int CreateGameObject(std::string Name = "[New GameObject]", RO_Type ObjectType = RO_Default) {
	int i = Scene.size();
	GameObject OBJ = GameObject(Name, i);
	Scene.push_back(OBJ);
	switch (ObjectType)
	{
		case RO_Default:
			break;
		case RO_UI:
			MakeGameObjectUI__(i);
			break;
		case RO_Phys:
			MakeGameObjectPhysical__(i);
			break;
		default:
			break;
	}
	return OBJ.GetID();
}