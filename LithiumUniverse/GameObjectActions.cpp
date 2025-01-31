#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>

#include <string>
#include <vector>
#include <box2d/box2d.h>

#include "GameObjectActions.h";
#include "GlobalResources.h";
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
	if (i > 0) {
		b2ShapeId* shapes = new b2ShapeId[i];
		b2Body_GetShapes(Body, shapes, i);
		for (int j = 0; j < i; ++j) {
			b2ShapeId s = shapes[j];
			b2DestroyShape(s, true);
		}
		delete[] shapes;
	}
}

/* Обновить коллизию объекта (private) */
void RefreshGameObjectCollider__(GameObject& OBJ) {
	RemoveGameObjectCollider__(OBJ);

	ColliderType CT = OBJ.Collider;
	if (!OBJ.Active) { CT = CT_None; }

	b2ShapeDef ShapeInfo = b2DefaultShapeDef();

	/* Какая коллизия    */
	ShapeInfo.filter.categoryBits = CT == CT_None ? F_Disabled : (OBJ.Static ? F_World             : F_Default          );

	/* С кем коллайдится */
	ShapeInfo.filter.maskBits     = CT == CT_None ? F_None     : (OBJ.Static ? F_World | F_Default : F_World | F_Default);

	switch (CT)
	{
	case CT_Box:
		b2Polygon ColBox = b2MakeBox(OBJ.SizeVisual.x, OBJ.SizeVisual.y);
		b2CreatePolygonShape(Bodies[OBJ.BodyID],
			&ShapeInfo,
			&ColBox
		);
		break;
	case CT_Circle:
		b2Circle ColCircle = b2Circle(b2Vec2(0, 0), OBJ.SizeVisual.x);
		b2CreateCircleShape(Bodies[OBJ.BodyID],
			&ShapeInfo,
			&ColCircle
		);
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

/* Клонировать данные одного игрового объекта в другой (private) */
void CloneGameObjectValuesFromOther__(GameObject& A, GameObject& B) {
	if (!A.Deleted && !B.Deleted) {
		A.Name = B.Name;
		A.Type = B.Type;
		A.Active = B.Active;
		A.DontDelete = B.DontDelete;
		A.BaseTextureRes = B.BaseTextureRes;
		A.BaseTextureID = B.BaseTextureID;
		A.BaseShaderRes = B.BaseShaderRes;
		A.BaseShaderID = B.BaseShaderID;
		A.FontRes = B.FontRes;
		A.PositionVisual = B.PositionVisual;
		A.LinePositionVisual = B.LinePositionVisual;
		A.SizeVisual = B.SizeVisual;
		A.OrientationVisual = B.OrientationVisual;
		A.Color = B.Color;
		A.Layer = B.Layer;
		A.Render = B.Render;
		A.RenderType = B.RenderType;
		A.Selectable = B.Selectable;
		A.Resize = B.Resize;
		A.Text = B.Text;
		A.Static = B.Static;
		A.Collider = B.Collider;

		int i = A.GetID();

		if (B.Type == RO_Phys) {
			SetGameObjectTransform(i,B.PositionVisual, B.OrientationVisual);
			SetGameObjectStatic(i, B.Static);
			SetGameObjectCollider(i, B.Collider);
		}
	}
	else {
		Error("GAMEOBJ", "Cannot clone data from [" + A.ToString() + "] to [" + B.ToString() + "] because one of the objects has been deleted! CloneGameObjectValuesToOther__(" + A.ToString() + "," + B.ToString() + ");");
	}
}

glm::vec2 BVec2ToVec2(const b2Vec2 v) {
	return glm::vec2(v.x, v.y);
}
b2Vec2 Vec2ToBVec2(const glm::vec2 v) {
	return b2Vec2(v.x, v.y);
}
b2Rot Makeb2Rot(const float o) {
	return b2Rot(cosf(-o), sinf(o));
}
float MakeOrientation(const b2Rot r) {
	return -atan2(r.s, r.c);
}

b2BodyId GetBody(const int i) {
#ifdef NDEBUG
	return Bodies[i];
#else
	if (i >= 0 && i <= Bodies.size()) {
		return Bodies[i];
	}
	else {
		Error("GAMEOBJ/BODY", "It is not possible to get the b2BodyId because it cannot be found by ID in the bodies! GetBody(" + std::to_string(i) + ");");
		return b2_nullBodyId;
	}
#endif
}

bool CheckOutSceneIndex(const int i) {
	return !(i >= 0 && i < Scene.size());
}

GameObject& GetGameObject(const int i, const std::string message) {
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
GameObject& GetGameObjectFromBody(const b2BodyId b) {
	auto it = std::find_if(Bodies.begin(), Bodies.end(),
		[&b](const b2BodyId& body) { return body.index1 == b.index1; });

	if (it != Bodies.end()) {
		int BodyID = std::distance(Bodies.begin(), it);
		auto it2 = std::find_if(Scene.begin(), Scene.end(),
			[&BodyID](const GameObject& gb) { return gb.BodyID == BodyID; });

		if (it2 != Scene.end()) {
			return GetGameObject(std::distance(Scene.begin(), it2), "GetGameObjectFromBody(?);");
		}
		else {
			Error("GAMEOBJ", "GameObject not found via b2BodyId! GetGameObjectFromBody(?); #In Scene#");
			return ErrorGameObject;
		}
	}
	else {
		Error("GAMEOBJ", "GameObject not found via b2BodyId! GetGameObjectFromBody(?); #In Bodies#");
		return ErrorGameObject;
	}
}

/* Предупредить, что объект не подходит для физики (private) */
void GameObjectNotSuitableForPhysics__(GameObject& OBJ, const std::string Func) {
	WarnSerious("GAMEOBJ", "It is impossible to perform function " + Func + " because " + OBJ.ToString() + " is not for physics!");
}

/* Предупредить, что объект не подходит для всего, потому-что удалён (private) */
void GameObjectDeleted__(GameObject& OBJ, const std::string Func) {
	WarnSerious("GAMEOBJ", "It is impossible to perform function " + Func + " because " + OBJ.ToString() + " has been deleted!");
}

/* Сделать объект физичным (private) */
void MakeGameObjectPhysical__(const int i) {
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
int GetGameObjectFromPoint(const glm::vec2 PointPos) {
	int result = -1;

	b2World_OverlapPoint(World, Vec2ToBVec2(PointPos), b2Transform_identity, b2DefaultQueryFilter(), [](b2ShapeId shapeId, void* context) {
		b2BodyId bodyId = b2Shape_GetBody(shapeId);
		b2Body_SetAwake(bodyId, true);
		GameObject& OBJ = GetGameObjectFromBody(bodyId);

		if (OBJ.Active && OBJ.Selectable) {
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

/* Установить, удаляемый ли объект? */
void SetGameObjectDontDelete(const int i, const bool b) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectDontDelete(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	if (!OBJ.Deleted) {
		OBJ.DontDelete = b;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectDontDelete(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	}
}

/* Установить, менять ли размер объекту взависимости от размера экрана? */
void SetGameObjectResize(const int i, const bool b) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectResize(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	if (!OBJ.Deleted) {
		OBJ.Resize = b;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectResize(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	}
}

/* Установить визуальный слой объекта */
void SetGameObjectLayer(const int i, const float l) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectLayer(" + std::to_string(i) + "," + std::to_string(l) + ");");
	if (!OBJ.Deleted) {
		OBJ.Layer = l;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectLayer(" + std::to_string(i) + "," + std::to_string(l) + ");");
	}
}

/* Установить текст объекту */
void SetGameObjectText(const int i, const std::string t) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectText(" + std::to_string(i) + ",\"" + t + "\");");
	if (!OBJ.Deleted) {
		OBJ.Text = t;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectText(" + std::to_string(i) + ",\"" + t + "\");");
	}
}

/* Установить шрифт объекту */
void SetGameObjectFont(const int i, const int f) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectFont(" + std::to_string(i) + "," + std::to_string(f) + ");");
	if (!OBJ.Deleted) {
		OBJ.FontRes = f;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectFont(" + std::to_string(i) + "," + std::to_string(f) + ");");
	}
}

/* Установить шейдер объекту */
void SetGameObjectShader(const int i, const int s) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectShader(" + std::to_string(i) + "," + std::to_string(s) + ");");
	if (!OBJ.Deleted) {
		OBJ.BaseShaderRes = s;
		OBJ.BaseShaderID = GetResourceAssetID(s);
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectShader(" + std::to_string(i) + "," + std::to_string(s) + ");");
	}
}

/* Установить текстуру объекту */
void SetGameObjectTexture(const int i, const int t) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectTexture(" + std::to_string(i) + "," + std::to_string(t) + ");");
	if (!OBJ.Deleted) {
		OBJ.BaseTextureRes = t;
		OBJ.BaseTextureID = GetResourceAssetID(t);
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectTexture(" + std::to_string(i) + "," + std::to_string(t) + ");");
	}
}

/* Установить, ативный ли объект? */
void SetGameObjectActive(const int i, const bool b) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectActive(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	if (!OBJ.Deleted) {
		OBJ.Active = b;
		if (OBJ.Type == RO_Phys) {
			if (b) {
				b2Body_SetType(Bodies[OBJ.BodyID], OBJ.Static ? b2_staticBody : b2_dynamicBody);
			}
			else {
				b2Body_SetType(Bodies[OBJ.BodyID], b2_staticBody);
			}
			RefreshGameObjectCollider__(OBJ);
		}
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectActive(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	}
}

/* Установить тип коллизии */
void SetGameObjectCollider(const int i, const ColliderType CT) {
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
void SetGameObjectSelectable(const int i, const bool b) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectSelectable(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	if (!OBJ.Deleted) {
		OBJ.Selectable = b;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectSelectable(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	}
}

/* Получить цвет объекта */
glm::vec4 GetGameObjectColor(const int i) {
	return GetGameObject(i, "GetGameObjectColor(" + std::to_string(i) + ");").Color;
}

/* Установить цвет объекту */
void SetGameObjectColor(const int i, const glm::vec4 c) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectSize(" + std::to_string(i) + "," + ToStringVec4(c) + ");");
	if (!OBJ.Deleted) {
		OBJ.Color = c;
	}
	else {
		GameObjectDeleted__(OBJ, "SetGameObjectSize(" + std::to_string(i) + "," + ToStringVec4(c) + ");");
	}
}

/* Получить размер объекта */
glm::vec2 GetGameObjectSize(const int i) {
	return GetGameObject(i, "GetGameObjectSize(" + std::to_string(i) + ");").SizeVisual;
}

/* Установить размер объекту */
void SetGameObjectSize(const int i, const glm::vec2 s) {
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
void SetGameObjectRenderable(const int i, const bool b) {
	GetGameObject(i, "SetGameObjectRenderable(" + std::to_string(i) + "," + ToStringBool(b) + ");").Render = b;
}

/* Получить позицию объекта */
glm::vec2 GetGameObjectPosition(const int i) {
	return GetGameObject(i, "GetGameObjectPosition(" + std::to_string(i) + ");").PositionVisual;
}

/* Получить поворот объекта */
float GetGameObjectOrientation(const int i) {
	return GetGameObject(i, "GetGameObjectOrientation(" + std::to_string(i) + ");").OrientationVisual;
}

/* Установить позицию объекту */
void SetGameObjectPosition(const int i, const glm::vec2 p) {
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
void SetGameObjectOrientation(const int i, const float r) {
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
void SetGameObjectTransform(const int i, const glm::vec2 p, const float r) {
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
void SetGameObjectStatic(const int i, const bool b) {
	GameObject& OBJ = GetGameObject(i, "SetGameObjectStatic(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	if (!OBJ.Deleted && OBJ.Type == RO_Phys) {
		OBJ.Static = b;
		b2Body_SetType(Bodies[OBJ.BodyID], b ? b2_staticBody : b2_dynamicBody);
	}
	else {
		GameObjectNotSuitableForPhysics__(OBJ, "SetGameObjectStatic(" + std::to_string(i) + "," + ToStringBool(b) + ");");
	}
}

/* Обновить объекты после обновления ресурсов */
void UpdateGameObjectsFromUpdateResources() {
	for (GameObject& OBJ : Scene) {
		OBJ.BaseTextureID = GetResourceAssetID(OBJ.BaseTextureRes);
		OBJ.BaseShaderID = GetResourceAssetID(OBJ.BaseShaderRes);
	}
}

/* Уничтожить объект */
void DeleteGameObject(const int i, bool IgnoreError) {
	bool Out = CheckOutSceneIndex(i);
	if (IgnoreError && Out) { return; }
	GameObject& OBJ = GetGameObject(i, "DeleteGameObject(" + std::to_string(i) + ");");
	if (!OBJ.Deleted) {
		if (OBJ.Type == RO_Phys) {
			int BodyID = OBJ.BodyID;
			b2BodyId Body = GetBody(BodyID);
			b2DestroyBody(Body);
			Bodies[BodyID] = b2_nullBodyId;
		}

		OBJ.Delete();
	}
	else {
		if (!IgnoreError) {
			Warn("GAMEOBJ", "Cannot delete GameObject because it has already been deleted! DeleteGameObject(" + std::to_string(i) + ");");
		}
	}
}
void DeleteGameObject(const int i) {
	DeleteGameObject(i, true);
}

/* Клонировать объект */
int CloneGameObject(const int i) {
	if (CheckOutSceneIndex(i)) {
		Error("GAMEOBJ","Failed to clone GameObject because it was not found! CloneGameObject(" + std::to_string(i) + ");");
		return -1;
	}
	else {
		GameObject OBJ = GetGameObject(i, "CloneGameObject(" + std::to_string(i) + ");");
		int ClonedObject = CreateGameObject(OBJ.Name, OBJ.Type, OBJ.CreatedFromMods);
		GameObject OBJ1 = GetGameObject(ClonedObject, "CloneGameObject(" + std::to_string(i) + "); #CLONED#");
		CloneGameObjectValuesFromOther__(OBJ1, OBJ);
		return ClonedObject;
	}
}

/* Создать объект */
int CreateGameObject(const std::string Name = "[New GameObject]", const RO_Type ObjectType = RO_Default, bool Modded = false) {
	int i = Scene.size();
	if (i < 0) { Error("GAMEOBJ", "Invalid Scene [" + std::to_string(i) + "] size! CreateGameObject(\"" + Name + "\"," + std::to_string(ObjectType) + ");"); }
	GameObject OBJ = GameObject(Name, i);
	if (OBJ.GetID() < 0) { Error("GAMEOBJ", "Invalid GameObject ID [" + std::to_string(OBJ.GetID()) + "]! CreateGameObject(\"" + Name + "\"," + std::to_string(ObjectType) + ");"); }
	OBJ.CreatedFromMods = Modded;
	Scene.push_back(OBJ);

	SetGameObjectShader (OBJ.GetID(), GetResource("Base", "Shaders/Default.lu_shader"        ).ID);
	SetGameObjectTexture(OBJ.GetID(), GetResource("Base", "Textures/Error/NotSelected.png").ID);

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
			Error("GAMEOBJ","Object type (" + std::to_string(ObjectType) + ") not found! CreateGameObject(\"" + Name + "\"," + std::to_string(ObjectType) + ");");
			break;
	}
	return OBJ.GetID();
}
int CreateGameObject(const std::string Name = "[New GameObject]", const RO_Type ObjectType = RO_Default) {
	return CreateGameObject(Name, ObjectType, false);
}