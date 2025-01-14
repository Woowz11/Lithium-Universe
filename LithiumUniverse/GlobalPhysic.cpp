#include <vector>

#include "GlobalRender.h";
#include "RenderedObject.h";
#include "GameCamera.h";
#include "GameData.h";
#include "Collider.h";
#include "PhysicalObject.h";

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

/* ==== Детект коллизий ==== */

/* Проверка коллизии: Точка с точкой */
bool DC_PointToPoint(RenderedObject Point1, RenderedObject Point2) {
	return Point1.Position.x == Point2.Position.x && Point1.Position.y == Point2.Position.y;
}

/* Проверка коллизии: Точка с кругом */
bool DC_PointToCircle(RenderedObject Point, RenderedObject Circle) {
	float X  = Point.Position.x;
	float Y  = Point.Position.y;
	float CX = Circle.Position.x;
	float CY = Circle.Position.y;
	float R  = Circle.Size.x / 2;

	float DistX = X - CX;
	float DistY = Y - CY;
	float Distance = sqrt((DistX * DistX) + (DistY * DistY));

	return Distance <= R;
}

/* Проверка коллизии: Круг с кругом */
bool DC_CircleToCircle(RenderedObject Circle1, RenderedObject Circle2) {
	float C1X = Circle1.Position.x;
	float C1Y = Circle1.Position.y;
	float R1 = Circle1.Size.x / 2;

	float C2X = Circle2.Position.x;
	float C2Y = Circle2.Position.y;
	float R2  = Circle2.Size.x / 2;

	float DistX = C1X - C2X;
	float DistY = C1Y - C2Y;
	float Distance = sqrt((DistX * DistX) + (DistY * DistY));

	return Distance <= R1+R2;
}

/* Проверка коллизии: Точка с квадратом */
bool DC_PointToSquare(RenderedObject Point, RenderedObject Square) {
	glm::vec2 P = Square.GetPhysicalPosition();
	return 
		Point.Position.x >= P.x                 &&
		Point.Position.x <= P.x + Square.Size.x &&
		Point.Position.y >= P.y                 &&
		Point.Position.y <= P.y + Square.Size.y ;
}

/* ==== Физические действия ==== */

/* Объекты прикосаются с друг другом? */
bool ObjectCollide(RenderedObject OBJ1, RenderedObject OBJ2) {
	Collider COL1 = OBJ1.Col;
	Collider COL2 = OBJ2.Col;

	if (COL1.Type == CLDR_Point && COL2.Type == CLDR_Point) {
		return DC_PointToPoint(OBJ1, OBJ2);
	}

	if (COL1.Type == CLDR_Point && COL2.Type == CLDR_Circle) {
		return DC_PointToCircle(OBJ1, OBJ2);
	}
	if (COL2.Type == CLDR_Point && COL1.Type == CLDR_Circle) {
		return DC_PointToCircle(OBJ2, OBJ1);
	}

	if (COL1.Type == CLDR_Circle && COL2.Type == CLDR_Circle) {
		return DC_CircleToCircle(OBJ1, OBJ2);
	}

	if (COL1.Type == CLDR_Point && COL2.Type == CLDR_Square) {
		return DC_PointToSquare(OBJ1, OBJ2);
	}
	if (COL2.Type == CLDR_Point && COL1.Type == CLDR_Square) {
		return DC_PointToSquare(OBJ2, OBJ1);
	}

	return false;
}

/* Выполнить физику для объекта */
void Physic(RenderedObject& OBJ, std::vector<RenderedObject>& Scene) {
	//OBJ.AddRotation(dt * 20);
	//OBJ.Size = glm::vec2((OBJ.Orientation.z - 180) / 90, (OBJ.Orientation.z - 180) / 90);
	if (OBJ.Name == "test") {
		OBJ.Size = glm::vec2(1, sin(t/2)+2);
	}

	if (OBJ.Name == "test2") {
		OBJ.SetPosition(PhysicalMousePosition);
	}
	else {
		bool collide = ObjectCollide(OBJ, Scene[0]);//PointCollide(PhysicalMousePosition, glm::vec2(0.0001f, 0.0001f), OBJ);
		OBJ.Color = (collide ? glm::vec4(0, 1, 0, 1) : glm::vec4(1, 0, 0, 1));
	}
}

/* Создать сцену */
void CreateScene(std::vector<RenderedObject>& Scene) {
	int square_texture = 1;
	int circle_texture = 3;

	PhysicalObject Test2 = PhysicalObject("test2");
	Test2.BaseShader = 1;
	Test2.BaseTexture = circle_texture;
	Test2.Col = Collider(CLDR_Point);
	Test2.Color = glm::vec4(0,0,1,1);
	Test2.Layer = 100;
	Test2.Render = false;
	Scene.push_back(Test2);

	for (int i = 1; i < 10; i++) {
		for (int j = -1; j < 3; j++) {
			PhysicalObject Test3 = PhysicalObject("test3");
			Test3.BaseShader = 1;
			Test3.BaseTexture = square_texture;
			Test3.Position = glm::vec2((i - 5) * 3, j * 3);
			Test3.Col = Collider(CLDR_Square);
			Test3.Size = glm::vec2((float)i / 5) * glm::vec2(1,(float)(j+2)/2);
			Scene.push_back(Test3);
		}
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