#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>

#include <algorithm>
#include <vector>

#include <box2cpp/box2cpp.h>

#include "GameObjectActions.h";
#include "GlobalRender.h";
#include "GameObject.h";
#include "GameCamera.h";
#include "GameData.h";

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

/* Объект физичной мыши */
int MouseObject = -1;

/* Создать мир для Box2D */
void InstallBox2D() {
	b2Version B2Version = b2GetVersion();
	Print("Box2D", "Box2D (" + std::to_string(B2Version.major) + "." + std::to_string(B2Version.minor) + "." + std::to_string(B2Version.revision) + ")");

	World = new b2::World(b2::World::Params{});
	World->SetGravity(b2Vec2(0, -9.8f));
}

/* Обновить данные с физики объекта */
void UpdatePhysicObject(GameObject& OBJ) {
	b2::Body& Body = GetBody(OBJ.BodyID);

	OBJ.PositionVisual = BVec2ToVec2(Body.GetPosition());
	OBJ.OrientationVisual = MakeOrientation(Body.GetRotation());
}

/* Выполнять после обновления физики */
void AfterUpdatePhysic() {
	//SetGameObjectPosition(MouseObject, MouseWorldPosition);
	//b2Polygon  b2MakeSquare(0.0001f);

	b2AABB MouseDetector;
	MouseDetector.lowerBound = b2Vec2(MouseWorldPosition.x - 0.0001f, MouseWorldPosition.y - 0.0001f);
	MouseDetector.upperBound = b2Vec2(MouseWorldPosition.x + 0.0001f, MouseWorldPosition.y + 0.0001f);

	World->Overlap(MouseDetector, b2DefaultQueryFilter(), 
	[&](b2::ShapeRef shape)
	{
		(void)shape;
		GameObject& OBJ = GetGameObjectFromBodyRef(shape.GetBody());
		PrintFast("d",OBJ.Name);
		return true;
	});
}

void CreateTestObject(int type) {
	int box = CreateGameObject("box", true);
	SetGameObjectPosition(box, MouseWorldPosition);
	if (type == 1) {
		SetGameObjectStatic(box, true);
	}
}

/* Создать сцену */
void CreateScene() {
	//MouseObject = CreateGameObject("[LU] MouseObject", true);
	//SetGameObjectStatic(MouseObject, true);
	
	//SetGameObjectRenderable(MouseObject, false);

	int box = CreateGameObject("box", true);

	int platform = CreateGameObject("platform", true);
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
	World->Step(pdt, 4);
	for (GameObject& OBJ : Scene) {
		if (!OBJ.Deleted && OBJ.Type == RO_Phys) {
			UpdatePhysicObject(OBJ);
		}
	}
	AfterUpdatePhysic();
	return Scene;
}

/* Установить физику */
void InstallPhysic() {
	InstallBox2D();
	CreateScene();
}