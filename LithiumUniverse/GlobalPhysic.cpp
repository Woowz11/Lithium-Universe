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
#include "GlobalUI.h";
#include "GameData.h";

/* ==== Тестовые объекты ==== */

std::vector<int> CreatedTestGameObjects = {};

void RemoveTestObject(std::vector<int>::iterator i) {
	if (!CreatedTestGameObjects.empty()) {
		DeleteGameObject(*i);
		CreatedTestGameObjects.erase(i);
	}
}

void RemoveLastTestObject() {
	if (!CreatedTestGameObjects.empty()) {
		RemoveTestObject(CreatedTestGameObjects.end() - 1);
	}
}

void RemoveAllTestObject() {
	if (!CreatedTestGameObjects.empty()) {
		for (int i : CreatedTestGameObjects) {
			DeleteGameObject(i);
		}
		CreatedTestGameObjects = {};
	}
}

void CreateTestObject(int type) {
	int box = CreateGameObject("box", RO_Phys);
	SetGameObjectPosition(box, MouseWorldPosition);

	switch (type)
	{
	case 1:
		SetGameObjectStatic(box, true);
		break;
	case 2:
		SetGameObjectSize(box, glm::vec2(5, 0.25f));
		SetGameObjectTexture(box, GetResource("Base", "Textures/Cable.png").ID);
		break;
	case 3:
		SetGameObjectCollider(box, CT_Circle);
		SetGameObjectTexture(box, GetResource("Base", "Textures/Circle.png").ID);
		break;
	default:
		SetGameObjectTexture(box, GetResource("Base", "Textures/Default.png").ID);
		break;
	}

	CreatedTestGameObjects.push_back(box);
}

/* Скорость симуляции */
float SimulationSpeed = 1;
void SetSimulationSpeed(float sp) {
	SimulationSpeed = sp;
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

	auto i = std::find(CreatedTestGameObjects.begin(), CreatedTestGameObjects.end(), OBJ.GetID());
	if (!OBJ.DontDelete && i != CreatedTestGameObjects.end() && OBJ.PositionVisual.y < -300) {
		RemoveTestObject(i);
	}
}

/* Выполнять после обновления физики */
void AfterUpdatePhysic() {
	SetGameObjectPosition(MouseObjectConnector, MouseWorldPosition);
	MouseObject = GetGameObjectFromPoint(MouseWorldPosition);
}

/* Создать сцену */
void CreateScene() {
	MouseObjectConnector = CreateGameObject("[LU] MouseObjectConnector", RO_Phys);
	SetGameObjectStatic(MouseObjectConnector, true);
	SetGameObjectSelectable(MouseObjectConnector, false);
	SetGameObjectCollider(MouseObjectConnector, CT_None);
	SetGameObjectRenderable(MouseObjectConnector, false);
	SetGameObjectDontDelete(MouseObjectConnector, true);

	float yoffset = 87.5f;

	int platform = CreateGameObject("platform", RO_Phys);
	SetGameObjectStatic(platform, true);
	SetGameObjectPosition(platform, glm::vec2(0, -100 + yoffset));
	SetGameObjectSize(platform, glm::vec2(100, 10));
	SetGameObjectColor(platform, glm::vec4(0.125f, 0.125f, 0.125f, 1));

	platform = CreateGameObject("platform", RO_Phys);
	SetGameObjectStatic(platform, true);
	SetGameObjectPosition(platform, glm::vec2(-100, 0 + yoffset));
	SetGameObjectSize(platform, glm::vec2(100, 10));
	SetGameObjectColor(platform, glm::vec4(0.125f, 0.125f, 0.125f, 1));
	SetGameObjectOrientation(platform, glm::radians(90.0f));

	platform = CreateGameObject("platform", RO_Phys);
	SetGameObjectStatic(platform, true);
	SetGameObjectPosition(platform, glm::vec2(0, 100 + yoffset));
	SetGameObjectSize(platform, glm::vec2(100, 10));
	SetGameObjectColor(platform, glm::vec4(0.125f, 0.125f, 0.125f, 1));

	platform = CreateGameObject("platform", RO_Phys);
	SetGameObjectStatic(platform, true);
	SetGameObjectPosition(platform, glm::vec2(100, 0 + yoffset));
	SetGameObjectSize(platform, glm::vec2(100, 10));
	SetGameObjectColor(platform, glm::vec4(0.125f, 0.125f, 0.125f, 1));
	SetGameObjectOrientation(platform, glm::radians(90.0f));
}

/* Обновить физику */
std::vector<GameObject>& UpdatePhysic() {
	float step = GameInFocus ? GameDeltaTime : 0;
	b2World_Step(World, step, 4);
	for (GameObject& OBJ : Scene) {
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
		}
	}
	AfterUpdatePhysic();
	AfterUpdateUI();
	return Scene;
}

/* Очистить физику */
void ClearPhysic() {
	ClearBox2D();
}

/* Установить физику */
void InstallPhysic() {
	InstallBox2D();
	CreateUI();
	CreateScene();
}