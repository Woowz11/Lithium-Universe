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

/* Обновить данные с физики объекта */
void UpdatePhysicObject(GameObject& OBJ) {
	b2Vec2 NewPosition = Bodies[OBJ.BodyID].GetPosition();
	OBJ.Position = glm::vec2(NewPosition.x, NewPosition.y);
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
		b2MakeBox(1,1)
	);
	Bodies[BodyID].SetTransform(b2Vec2(OBJ.Position.x, OBJ.Position.y), b2Rot_identity);
}

/* Создать сцену */
void CreateScene() {
	GameObject Test = GameObject("test");
	Test.BaseShader = 1;
	Test.BaseTexture = 1;
	MakeGameObjectPhysical(Test);
	Scene.push_back(Test);

	GameObject Test2 = GameObject("test2");
	Test2.BaseShader = 1;
	Test2.BaseTexture = 1;
	Test2.Position = glm::vec2(0, -3);
	Test2.Size = glm::vec2(10, 1);
	//Test2.SetRotation(glm::radians(45.0f));
	Test2.Static = true;
	MakeGameObjectPhysical(Test2);
	Scene.push_back(Test2);
}

/* Указать позицию курсора */
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2) {
	
}

/* Обновить физику */
std::vector<GameObject>& UpdatePhysic() {
	World->Step(1/60.0f/60.0f, 4);
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