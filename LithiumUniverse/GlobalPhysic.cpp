#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>

#include <algorithm>
#include <vector>

#include <box2cpp/box2cpp.h>

#include "GlobalRender.h";
#include "GameObject.h";
#include "GameCamera.h";
#include "GameData.h";

/* ==== Сцена ==== */

/* Сцена */
std::vector<GameObject> Scene = {};

/* Физичные объекты */
std::vector<b2::Body> Bodies = {};

/* Скорость симуляции */
float SimulationSpeed = 1;
void SetSimulationSpeed(float sp) {
	SimulationSpeed = sp;
}
float GetSimulationSpeed() {
	return SimulationSpeed;
}

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

/* Обновить данные с физики объекта */
void UpdatePhysicObject(GameObject& OBJ) {
	b2::Body& Body = Bodies[OBJ.BodyID];

	b2Vec2 NewPosition = Body.GetPosition();
	OBJ.Position = glm::vec2(NewPosition.x, NewPosition.y);
	b2Rot NewOrientation = Body.GetRotation();
	OBJ.Orientation = -NewOrientation.s;
}

/* Создать мир для Box2D */
void InstallBox2D() {
	World = new b2::World(b2::World::Params{});
	//World->SetGravity(b2Vec2(0, -10));
}

/* Сделать объект физичным */
void MakeGameObjectPhysical(GameObject& OBJ) {
	OBJ.Type = RO_Phys;
	OBJ.Selectable = true;

	int BodyID = Bodies.size();
	OBJ.BodyID = BodyID;

	b2::Body::Params P;
	if (!OBJ.Static) {
		P.type = b2_dynamicBody;
	}
	Bodies.push_back(World->CreateBody(b2::OwningHandle, P));
	Bodies[BodyID].CreateShape(
		b2::DestroyWithParent,
		b2::Shape::Params{},
		b2MakeBox(OBJ.Size.x/2, OBJ.Size.y/2)
	);
	Bodies[BodyID].SetTransform(b2Vec2(OBJ.Position.x, OBJ.Position.y), b2Rot(1,-OBJ.Orientation));
}

void CreateTestObject() {
	GameObject Test = GameObject("test");
	Test.BaseShader = 1;
	Test.BaseTexture = 1;
	Test.Position = MouseWorldPosition;
	MakeGameObjectPhysical(Test);
	Scene.push_back(Test);
}

/* Создать сцену */
void CreateScene() {
	GameObject Test = GameObject("test");
	Test.BaseShader = 1;
	Test.BaseTexture = 1;
	Test.Position = glm::vec2(0, 10);
	MakeGameObjectPhysical(Test);
	Scene.push_back(Test);

	GameObject Test2 = GameObject("test2");
	Test2.BaseShader = 1;
	Test2.BaseTexture = 1;
	Test2.Position = glm::vec2(0, -3);
	Test2.Size = glm::vec2(10, 1);
	Test2.SetRotation(glm::radians(15.0f));
	Test2.Static = true;
	MakeGameObjectPhysical(Test2);
	Scene.push_back(Test2);
}

/* Указать позицию курсора */
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2) {
	MouseWorldPosition = ScreenPositionToWorld(Pos2);
}

/* Обновить физику */
std::vector<GameObject>& UpdatePhysic() {
	World->Step(pdt, 4);
	for (GameObject& OBJ : Scene) {
		if (OBJ.Type == RO_Phys) {
			UpdatePhysicObject(OBJ);
		}
	}
	return Scene;
}

/* Установить физику */
void InstallPhysic() {
	InstallBox2D();
	CreateScene();
}