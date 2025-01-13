#include <vector>

#include "GlobalRender.h";
#include "RenderedObject.h";
#include "GameCamera.h";
#include "GameData.h";

float dt = 0;
float t = 0;
void UpdateDeltaTime_PHYSIC(float DT, float Time_) {
	dt = DT;
	t = Time_;
}

/* Установить физику */
void InstallPhysic() {

}

/* ==== Физические действия ==== */

float FixObjectColliderNumber(float y) {
	return -y/2 + 0.5f;
}

/* Объекты прикосаются с друг другом? */
bool ObjectCollider(RenderedObject OBJ1, RenderedObject OBJ2) {
	glm::vec2 OBJ1Size = OBJ1.Size;
	glm::vec2 OBJ2Size = OBJ2.Size;
	glm::vec2 OBJ1Pos  = OBJ1.Position + glm::vec2(FixObjectColliderNumber(OBJ1Size.x), FixObjectColliderNumber(OBJ1Size.y));
	glm::vec2 OBJ2Pos = OBJ2.Position + glm::vec2(FixObjectColliderNumber(OBJ2Size.x), FixObjectColliderNumber(OBJ2Size.y));

	bool CollideX = 
		OBJ1Pos.x + OBJ1Size.x >= OBJ2Pos.x
		&&
		OBJ2Pos.x + OBJ2Size.x >= OBJ1Pos.x;
	bool CollideY = 
		OBJ1Pos.y + OBJ1Size.y >= OBJ2Pos.y
		&&
		OBJ2Pos.y + OBJ2Size.y >= OBJ1Pos.y;

	return CollideX && CollideY;
}

/* Выполнить физику для объекта */
void Physic(RenderedObject& OBJ, std::vector<RenderedObject>& Scene) {
	//OBJ.AddRotation(dt * 20);
	//OBJ.Size = glm::vec2((OBJ.Orientation.z - 180) / 90, (OBJ.Orientation.z - 180) / 90);
	if (OBJ.Name == "test") {
		OBJ.Size = glm::vec2(1, sin(t/2)+2);
	}
	if (OBJ.Name == "test2") {
		OBJ.SetPosition(-(Camera->Position));
		OBJ.Color = (ObjectCollider(OBJ, Scene[0]) ? glm::vec4(0, 1, 0, 1) : glm::vec4(1, 0, 0, 1) );
	}
}

/* Создать сцену */
void CreateScene(std::vector<RenderedObject>& Scene) {
	RenderedObject Test = RenderedObject("test");
	Test.BaseShader = 1;
	Test.BaseTexture = 1;
	//Test.Orientation = glm::vec3(0, 0, 45);
	Test.Size = glm::vec2(1, 1);
	Test.Position = glm::vec2(-3, 3);
	Scene.push_back(Test);

	RenderedObject Test2 = RenderedObject("test2");
	Test2.BaseShader = 1;
	Test2.BaseTexture = 1;
	//Test2.Color = glm::vec4(1,0.5f,0.5f,1);
	Scene.push_back(Test2);
}

/* Обновить физику */
void UpdatePhysic(std::vector<RenderedObject>& Scene) {
	for (RenderedObject& OBJ : Scene) {
		Physic(OBJ,Scene);
	}
}