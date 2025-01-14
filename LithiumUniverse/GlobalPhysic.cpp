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

glm::vec2 PhysicalMousePosition = glm::vec2(0, 0);

/* Установить физику */
void InstallPhysic() {

}

/* ==== Физические действия ==== */

/* Неизвестная, загадачная формула, без неё детект коллайдеров кривой */
float FixObjectColliderNumber(float y) {
	return -y / 2.0f + 0.5f;
}

/* Точка прикосается с объектом? */
bool PointCollide(glm::vec2 Position, glm::vec2 Size, RenderedObject OBJ) {
	glm::vec2 P = Position + glm::vec2(FixObjectColliderNumber(Size.x), FixObjectColliderNumber(Size.y));

	glm::vec2 OBJSize = OBJ.Size;
	glm::vec2 OBJPos = OBJ.Position + glm::vec2(FixObjectColliderNumber(OBJSize.x), FixObjectColliderNumber(OBJSize.y));

	bool CollideX =
		P.x + Size.x >= OBJPos.x
		&&
		OBJPos.x + OBJSize.x >= P.x;
	bool CollideY =
		P.y + Size.y >= OBJPos.y
		&&
		OBJPos.y + OBJSize.y >= P.y;

	return CollideX && CollideY;
}

/* Объекты прикосаются с друг другом? */
bool ObjectCollide(RenderedObject OBJ1, RenderedObject OBJ2) {
	glm::vec2 OBJ1Size = OBJ1.Size;
	glm::vec2 OBJ2Size = OBJ2.Size;
	glm::vec2 OBJ1Pos  = OBJ1.Position + glm::vec2(FixObjectColliderNumber(OBJ1Size.x), FixObjectColliderNumber(OBJ1Size.y));
	glm::vec2 OBJ2Pos  = OBJ2.Position + glm::vec2(FixObjectColliderNumber(OBJ2Size.x), FixObjectColliderNumber(OBJ2Size.y));

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
	//if (OBJ.Name == "test2") {
		//OBJ.SetPosition(-(Camera->Position));

	//}
		if (OBJ.Name == "test2") {
			OBJ.SetPosition(PhysicalMousePosition);
		}
		else {
			bool collide = PointCollide(PhysicalMousePosition, glm::vec2(0.0001f, 0.0001f), OBJ);//ObjectCollide(OBJ, Scene[0]);
			OBJ.Color = (collide ? glm::vec4(0, 1, 0, 1) : glm::vec4(1, 0, 0, 1));
		}
}

/* Создать сцену */
void CreateScene(std::vector<RenderedObject>& Scene) {
	RenderedObject Test = RenderedObject("test");
	Test.BaseShader = 1;
	Test.BaseTexture = 1;
	//Test.Orientation = glm::vec3(0, 0, 45);
	//Test.Size = glm::vec2(1, 1);
	//Test.Position = glm::vec2(-3, 3);
	Test.Active = false;
	Scene.push_back(Test);

	RenderedObject Test2 = RenderedObject("test2");
	Test2.BaseShader = 1;
	Test2.BaseTexture = 1;
	//-10.0f/3
	Test2.Size = glm::vec2(0.0001f,0.0001f);
	Test2.Color = glm::vec4(0,0,1,1);
	Test2.Layer = 100;
	Scene.push_back(Test2);

	int x = -5;
	int y = -5;
	for (int i = 0; i < 100; i++) {
		x++;
		if (x >= 10) {
			x = -5;
			y++;
		}

		RenderedObject Test3 = RenderedObject("test3");
		Test3.BaseShader = 1;
		Test3.BaseTexture = 1;
		Test3.Position = glm::vec2(x,y);
		Scene.push_back(Test3);
	}
}

/* Указать позицию курсора */
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2) {
	PhysicalMousePosition = ScreenPositionToWorld(Pos2);
	//PrintFast("mouse", std::to_string(PhysicalMousePosition.x) + " | " + std::to_string(PhysicalMousePosition.y));
}

/* Обновить физику */
void UpdatePhysic(std::vector<RenderedObject>& Scene) {
	for (RenderedObject& OBJ : Scene) {
		if (OBJ.Active) {
			Physic(OBJ, Scene);
		}
	}
}