#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>

#include <algorithm>
#include <vector>

#include <box2d/box2d.h>

#include "GameObjectActions.h";
#include "GlobalResources.h";
#include "GlobalRender.h";
#include "GameObject.h";
#include "GameCamera.h";
#include "GlobalLua.h";
#include "GlobalUI.h";
#include "GameData.h";

/* ==== Тестовые объекты ==== */

/* Скорость симуляции */
float SimulationSpeed = 1;
void SetSimulationSpeed(float sp) {
	if (sp >= 0) {
		SimulationSpeed = sp;
	}
	else {
		Error("GAME","Cannot set simulation speed < 0! SetSimulationSpeed(" + std::to_string(sp) + ");");
	}
}
float GetSimulationSpeed() {
	return SimulationSpeed;
}

/* Объект физичной мыши */
int MouseObjectConnector = -1;

/* Объект на который наведена мышь */
int MouseObject = -1;

/* Очистить Box2D */
void ClearBox2D() {
	b2DestroyWorld(World);
}

/* Создать мир для Box2D */
void InstallBox2D() {
	b2Version B2Version = b2GetVersion();
	Print("Box2D", "Box2D (" + std::to_string(B2Version.major) + "." + std::to_string(B2Version.minor) + "." + std::to_string(B2Version.revision) + ")");

	b2WorldDef WorldInfo = b2DefaultWorldDef();
	WorldInfo.gravity = b2Vec2(0, -9.8f);
	World = b2CreateWorld(&WorldInfo);
}

/* Обновить данные с физики объекта */
void UpdatePhysicObject(GameObject& OBJ) {
	b2BodyId Body = GetBody(OBJ.BodyID);

	OBJ.PositionVisual = BVec2ToVec2(b2Body_GetPosition(Body));
	OBJ.OrientationVisual = MakeOrientation(b2Body_GetRotation(Body));
}

/* Выполнять после обновления физики */
void AfterUpdatePhysic() {
	SetGameObjectPosition(MouseObjectConnector, MouseWorldPosition);
	MouseObject = GetGameObjectFromPoint(MouseWorldPosition);
}

/* Создать сцену */
void CreateScene(Scenes Scen) {
	MouseObjectConnector = CreateGameObject("[LU] MouseObjectConnector", RO_Phys);
	SetGameObjectStatic(MouseObjectConnector, true);
	SetGameObjectSelectable(MouseObjectConnector, false);
	SetGameObjectCollider(MouseObjectConnector, CT_None);
	SetGameObjectRenderable(MouseObjectConnector, false);
	SetGameObjectDontDelete(MouseObjectConnector, true);

	switch (Scen)
	{
		case SCENE_Game: {

			float yoffset = 87.5f;

			int platform = CreateGameObject("platform", RO_Phys);
			SetGameObjectTexture(platform, GetResource("Base:Textures/Blank.png").ID);
			SetGameObjectStatic(platform, true);
			SetGameObjectPosition(platform, glm::vec2(0, -100 + yoffset));
			SetGameObjectSize(platform, glm::vec2(100, 10));
			SetGameObjectColor(platform, glm::vec4(0.125f, 0.125f, 0.125f, 1));

			platform = CreateGameObject("platform", RO_Phys);
			SetGameObjectTexture(platform, GetResource("Base:Textures/Blank.png").ID);
			SetGameObjectStatic(platform, true);
			SetGameObjectPosition(platform, glm::vec2(-100, 0 + yoffset));
			SetGameObjectSize(platform, glm::vec2(100, 10));
			SetGameObjectColor(platform, glm::vec4(0.125f, 0.125f, 0.125f, 1));
			SetGameObjectOrientation(platform, glm::radians(90.0f));

			platform = CreateGameObject("platform", RO_Phys);
			SetGameObjectTexture(platform, GetResource("Base:Textures/Blank.png").ID);
			SetGameObjectStatic(platform, true);
			SetGameObjectPosition(platform, glm::vec2(0, 100 + yoffset));
			SetGameObjectSize(platform, glm::vec2(100, 10));
			SetGameObjectColor(platform, glm::vec4(0.125f, 0.125f, 0.125f, 1));

			platform = CreateGameObject("platform", RO_Phys);
			SetGameObjectTexture(platform, GetResource("Base:Textures/Blank.png").ID);
			SetGameObjectStatic(platform, true);
			SetGameObjectPosition(platform, glm::vec2(100, 0 + yoffset));
			SetGameObjectSize(platform, glm::vec2(100, 10));
			SetGameObjectColor(platform, glm::vec4(0.125f, 0.125f, 0.125f, 1));
			SetGameObjectOrientation(platform, glm::radians(90.0f));
			
			break;
		}
		default: {
			break;
		}
	}
}

/* Обновить физику */
int timer = 0;
std::vector<GameObject>& UpdatePhysic() {
	float step = GameInFocus ? GameDeltaTime : 0;
	if (step > 0) {
		b2World_Step(World, step, 4);
	}

	bool callLuaEvents = (timer > 10);

	std::vector<int> LuaEventGameObjects = {};
	for (size_t i = 0; i < Scene.size(); i++) {
		GameObject& OBJ = Scene[i];
		int ID = OBJ.GetID();

		if (!OBJ.Deleted && OBJ.Active) {
			switch (OBJ.Type)
			{
			case RO_Phys:
				UpdatePhysicObject(OBJ);
				break;
			case RO_UI:
				UpdateUI(OBJ);
				break;
			default:
				break;
			}

			if (callLuaEvents && OBJ.CreatedFromMods) {
				LuaEventGameObjects.push_back(ID);
			}
		}
	}
	AfterUpdatePhysic();
	AfterUpdateUI();

	if (callLuaEvents) {
		for (const sol::function& F : LUA_Events_UpdateEveryGameObject) {
			if (F.valid()) {
				F(LuaEventGameObjects);
			}
		}
		timer = 0;
	}
	timer++;

	for (const sol::function& F : LUA_Events_Update) {
		F();
	}

	return Scene;
}

/* Очистить физику */
void ClearPhysic() {
	ClearBox2D();
}

/* Установить физику */
void InstallPhysic() {
	InstallBox2D();
	SetScene(SCENE_MainMenu);
}