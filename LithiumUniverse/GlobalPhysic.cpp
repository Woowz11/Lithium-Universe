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

/* Проверка коллизии: Точка с кругом (Функция) */
bool DC_PointToCircle_Func(glm::vec2 PointPos, glm::vec2 CirclePos, float Radius) {
	float X = PointPos.x;
	float Y = PointPos.y;
	float CX = CirclePos.x;
	float CY = CirclePos.y;
	float R = Radius / 2;

	float DistX = X - CX;
	float DistY = Y - CY;
	float Distance = sqrt((DistX * DistX) + (DistY * DistY));

	return Distance <= R;
}

/* Проверка коллизии: Точка с кругом */
bool DC_PointToCircle(RenderedObject Point, RenderedObject Circle) {
	return DC_PointToCircle_Func(Point.Position, Circle.Position, Circle.Size.x);
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

/* Проверка коллизии: Квадрат с квадратом */
bool DC_SquareToSquare(RenderedObject Square1, RenderedObject Square2) {
	glm::vec2 P1 = Square1.GetPhysicalPosition();
	glm::vec2 P2 = Square2.GetPhysicalPosition();
	return
		P1.x + Square1.Size.x >= P2.x &&
		P1.y + Square1.Size.y >= P2.y &&
		P2.x + Square2.Size.x >= P1.x &&
		P2.y + Square2.Size.y >= P1.y ;
}

/* Проверка коллизии: Круг с квадратом */
bool DC_CircleToSquare(RenderedObject Circle, RenderedObject Square) {
	float CX = Circle.Position.x;
	float CY = Circle.Position.y;
	float R  = Circle.Size.x / 2;

	glm::vec2 P = Square.GetPhysicalPosition();

	float TestX = CX;
	float TestY = CY;

	if (CX < P.x) {
		TestX = P.x;
	}
	else if (CX > P.x + Square.Size.x) {
		TestX = P.x + Square.Size.x;
	}

	if (CY < P.y) {
		TestY = P.y;
	}
	else if (CY > P.y + Square.Size.y) {
		TestY = P.y + Square.Size.y;
	}

	float DistX = CX - TestX;
	float DistY = CY - TestY;
	float Distance = sqrt((DistX*DistX) + (DistY*DistY));

	return Distance <= R;
}

/* Проверка коллизии: Точка с линией (Функция) */
bool DT_PointToLine_Func(glm::vec2 P, glm::vec2 SP, glm::vec2 EP) {
	float Accurate = 0.0001f;

	float Dist1 = glm::distance(P, SP);
	float Dist2 = glm::distance(P, EP);

	float LineLength = glm::distance(SP, EP);

	return (Dist1 + Dist2 >= LineLength - Accurate && Dist1 + Dist2 <= LineLength + Accurate);
}

/* Проверка коллизии: Точка с линией */
bool DT_PointToLine(RenderedObject Point, RenderedObject Line) {
	return DT_PointToLine_Func(Point.Position, Line.GetLineStartPosition(), Line.GetLineEndPosition());
}

/* Проверка коллизии: Круг с линией */
bool DT_CircleToLine(RenderedObject Circle, RenderedObject Line) {
	glm::vec2 StartPos = Line.GetLineStartPosition();
	glm::vec2 EndPos = Line.GetLineEndPosition();

	glm::vec2 P = Circle.Position;
	float R = Circle.Size.x;

	bool Inside1 = DC_PointToCircle_Func(StartPos, P, R);
	bool Inside2 = DC_PointToCircle_Func(EndPos, P, R);

	if (Inside1 || Inside2) { return true; }

	float DistX = StartPos.x - EndPos.x;
	float DistY = StartPos.y - EndPos.y;
	float Distance = sqrt((DistX * DistX) + (DistY * DistY));

	float Dot = ( (((P.x - StartPos.x)*(EndPos.x - StartPos.x)) + ((P.y - StartPos.y)*(EndPos.y - StartPos.y))) / pow(Distance, 2) );

	float ClosestX = StartPos.x + (Dot * (EndPos.x - StartPos.x));
	float ClosestY = StartPos.y + (Dot * (EndPos.y - StartPos.y));

	bool OnSegment = DT_PointToLine_Func(glm::vec2(ClosestX, ClosestY), StartPos, EndPos);

	if (!OnSegment) { return false; }

	DistX = ClosestX - P.x;
	DistY = ClosestY - P.y;
	Distance = sqrt((DistX * DistX) + (DistY * DistY));

	return Distance <= R;
}

/* ==== Физические действия ==== */

/* Объекты прикосаются с друг другом? */
bool ObjectCollide(RenderedObject OBJ1, RenderedObject OBJ2) {
	Collider COL1 = OBJ1.Col;
	Collider COL2 = OBJ2.Col;

	if (COL1.Type == CLDR_None || COL2.Type == CLDR_None) {
		return false;
	}

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

	if (COL1.Type == CLDR_Square && COL2.Type == CLDR_Square) {
		return DC_SquareToSquare(OBJ1, OBJ2);
	}

	if (COL1.Type == CLDR_Circle && COL2.Type == CLDR_Square) {
		return DC_CircleToSquare(OBJ1, OBJ2);
	}
	if (COL2.Type == CLDR_Circle && COL1.Type == CLDR_Square) {
		return DC_CircleToSquare(OBJ2, OBJ1);
	}

	if (COL1.Type == CLDR_Point && COL2.Type == CLDR_Line) {
		return DT_PointToLine(OBJ1, OBJ2);
	}
	if (COL2.Type == CLDR_Point && COL1.Type == CLDR_Line) {
		return DT_PointToLine(OBJ2, OBJ1);
	}

	if (COL1.Type == CLDR_Circle && COL2.Type == CLDR_Line) {
		return DT_CircleToLine(OBJ1, OBJ2);
	}
	if (COL2.Type == CLDR_Circle && COL1.Type == CLDR_Line) {
		return DT_CircleToLine(OBJ2, OBJ1);
	}

	return false;
}

/* Выполнить физику для объекта */
void Physic(RenderedObject& OBJ, std::vector<RenderedObject>& Scene) {
	if (OBJ.Name == "test2") {
		OBJ.SetPosition(PhysicalMousePosition);
	}
	else {
		bool collide = ObjectCollide(OBJ, Scene[0]);
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
	Test2.Col = Collider(CLDR_Circle);
	Test2.Color = glm::vec4(0,0,1,1);
	Test2.Layer = 100;
	//Test2.Render = false;
	Scene.push_back(Test2);
	
	PhysicalObject Line = PhysicalObject("line");
	Line.BaseTexture = 4;
	Line.MakeItLine(glm::vec2(-5, -5), glm::vec2(5, 5), 0.1f);
	Line.Col = Collider(CLDR_Line);
	Scene.push_back(Line);

	PhysicalObject Line2 = PhysicalObject("line");
	Line2.BaseTexture = 4;
	Line2.MakeItLine(glm::vec2(-5, 5), glm::vec2(5, 10), 1.0f);
	Line2.Col = Collider(CLDR_Line);
	Scene.push_back(Line2);

	PhysicalObject Line3 = PhysicalObject("line");
	Line3.BaseTexture = 4;
	Line3.MakeItLine(glm::vec2(-5, -10), glm::vec2(5, 0), 0.01f);
	Line3.Col = Collider(CLDR_Line);
	Scene.push_back(Line3);
}

/* Указать позицию курсора */
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2) {
	PhysicalMousePosition = ScreenPositionToWorld(Pos2);
}

/* Обновить физику */
void UpdatePhysic(std::vector<RenderedObject>& Scene) {
	for (RenderedObject& OBJ : Scene) {
		if (OBJ.Active) {
			Physic(OBJ, Scene);
		}
	}
}