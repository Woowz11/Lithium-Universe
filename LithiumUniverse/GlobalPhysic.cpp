#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>

#include <algorithm>
#include <vector>

#include <box2d/box2d.h>

#include "GameObjectActions.h";
#include "GlobalRender.h";
#include "GameObject.h";
#include "GameCamera.h";
#include "GlobalUI.h";
#include "GameData.h";

/* Скорость симуляции */
float SimulationSpeed = 1;
void SetSimulationSpeed(float sp) {
	SimulationSpeed = sp;
}
float GetSimulationSpeed() {
	return SimulationSpeed;
}

/* Версия разработчика */
bool DeveloperVersion = false;

/* DeltaTime умноженный на скорость симуляции */
float pdt = 0;

/* DeltaTime */
float dt = 0;
void UpdateDeltaTimePhysic(float dt_) {
	dt = dt_;
	pdt = dt_ * SimulationSpeed;
}

/* Позиция мыши в мире */
glm::vec2 MouseWorldPosition = glm::vec2(0, 0);

/* Объект физичной мыши */
int MouseObjectConnector = -1;

/* Объект на который наведена мышь */
int MouseObject = -1;
int GetMouseObject() {
	return MouseObject;
}

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

void CreateTestObject(int type) {
	int box = CreateGameObject("box", RO_Phys);
	SetGameObjectPosition(box, MouseWorldPosition);
	if (type == 1) {
		SetGameObjectStatic(box, true);
	}
	else {
		SetGameObjectTexture(box, 2);
	}
}

/* Создать сцену */
void CreateScene() {
	MouseObjectConnector = CreateGameObject("[LU] MouseObjectConnector", RO_Phys);
	SetGameObjectStatic(MouseObjectConnector, true);
	SetGameObjectSelectable(MouseObjectConnector, false);
	SetGameObjectCollider(MouseObjectConnector, CT_None);
	SetGameObjectRenderable(MouseObjectConnector, false);

	int box = CreateGameObject("box", RO_Phys);

	int platform = CreateGameObject("platform", RO_Phys);
	SetGameObjectStatic(platform, true);
	SetGameObjectPosition(platform ,glm::vec2(0, -3));
	SetGameObjectSize(platform, glm::vec2(100, 1));
	SetGameObjectColor(platform, glm::vec4(0.125f, 0.125f, 0.125f, 1));
}

/* Указать позицию курсора */
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2) {
	MouseWorldPosition = ScreenPositionToWorld(Pos2);
}

/* Обновить физику */
std::vector<GameObject>& UpdatePhysic() {
	float step = GameInFocus ? pdt : 0;
	b2World_Step(World, step, 4);
	for (GameObject& OBJ : Scene) {
		if (!OBJ.Deleted && OBJ.Type == RO_Phys) {
			UpdatePhysicObject(OBJ);
		}
	}
	AfterUpdatePhysic();
	return Scene;
}

/* Очистить физику */
void ClearPhysic() {
	ClearBox2D();
}

/* Установить физику */
void InstallPhysic(bool DV) {
	DeveloperVersion = DV;
	InstallBox2D();
	CreateUI();
	CreateScene();
}