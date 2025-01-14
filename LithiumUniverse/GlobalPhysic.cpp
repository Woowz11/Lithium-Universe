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

/* Проверка коллизии: Линия с линией (Функция) */
bool DT_LineToLine_Func(glm::vec2 Start1Pos, glm::vec2 End1Pos, glm::vec2 Start2Pos, glm::vec2 End2Pos) {
	float UA =
		((End2Pos.x - Start2Pos.x) * (Start1Pos.y - Start2Pos.y) - (End2Pos.y - Start2Pos.y) * (Start1Pos.x - Start2Pos.x))
		/
		((End2Pos.y - Start2Pos.y) * (End1Pos.x - Start1Pos.x) - (End2Pos.x - Start2Pos.x) * (End1Pos.y - Start1Pos.y));

	float UB =
		((End1Pos.x - Start1Pos.x) * (Start1Pos.y - Start2Pos.y) - (End1Pos.y - Start1Pos.y) * (Start1Pos.x - Start2Pos.x))
		/
		((End2Pos.y - Start2Pos.y) * (End1Pos.x - Start1Pos.x) - (End2Pos.x - Start2Pos.x) * (End1Pos.y - Start1Pos.y));

	return (UA >= 0 && UA <= 1 && UB >= 0 && UB <= 1);
}

/* Проверка коллизии: Линия с линией */
bool DT_LineToLine(RenderedObject Line1, RenderedObject Line2) {
	glm::vec2 Start1Pos = Line1.GetLineStartPosition();
	glm::vec2 End1Pos   = Line1.GetLineEndPosition();
	glm::vec2 Start2Pos = Line2.GetLineStartPosition();
	glm::vec2 End2Pos   = Line2.GetLineEndPosition();

	return DT_LineToLine_Func(Start1Pos, End1Pos, Start2Pos, End2Pos);
}

/* Проверка коллизии: Квадрат с линией */
bool DT_SquareToLine(RenderedObject Square, RenderedObject Line) {
	glm::vec2 StartPos = Line.GetLineStartPosition();
	glm::vec2 EndPos   = Line.GetLineEndPosition();

	glm::vec2 P = Square.GetPhysicalPosition();
	glm::vec2 S = Square.Size;

	bool Left   = DT_LineToLine_Func(StartPos, EndPos, P, glm::vec2(P.x      , P.y + S.y)                                  );
	bool Right  = DT_LineToLine_Func(StartPos, EndPos,    glm::vec2(P.x + S.x, P.y      ),  glm::vec2(P.x + S.x, P.y + S.y));
	bool Top    = DT_LineToLine_Func(StartPos, EndPos, P, glm::vec2(P.x + S.x, P.y      )                                  );
	bool Bottom = DT_LineToLine_Func(StartPos, EndPos,    glm::vec2(P.x      , P.y + S.y),  glm::vec2(P.x + S.x, P.y + S.y));

	return Left || Right || Top || Bottom;
}

/* Проверка коллизии: Точка с кастомной */
bool DT_PointToCustom(RenderedObject Point, RenderedObject Custom) {
	glm::vec2 P = Point.Position;
	std::vector<glm::vec2> Points = Custom.Col.GetPhysicalPoints(Custom.Position, Custom.Size);

	bool Result = false;

	int size = Points.size();
	for (int i = 0, j = size-1; i < size; j = i++) {
		glm::vec2 VC = Points[i];
		glm::vec2 VN = Points[j];

		if (
			((VC.y > P.y) != (VN.y > P.y))
			&&
			(P.x < (VN.x - VC.x) * (P.y - VC.y) / (VN.y - VC.y) + VC.x)
		) {
			Result = !Result;
		}
	}

	return Result;
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

	if (COL1.Type == CLDR_Line && COL2.Type == CLDR_Line) {
		return DT_LineToLine(OBJ1, OBJ2);
	}

	if (COL1.Type == CLDR_Square && COL2.Type == CLDR_Line) {
		return DT_SquareToLine(OBJ1, OBJ2);
	}
	if (COL2.Type == CLDR_Square && COL1.Type == CLDR_Line) {
		return DT_SquareToLine(OBJ2, OBJ1);
	}

	if (COL1.Type == CLDR_Point && COL2.Type == CLDR_Custom) {
		return DT_PointToCustom(OBJ1, OBJ2);
	}
	if (COL2.Type == CLDR_Point && COL1.Type == CLDR_Custom) {
		return DT_PointToCustom(OBJ2, OBJ1);
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
	int special_texture = 2;
	int circle_texture = 3;
	int cable_texture = 4;

	PhysicalObject Test = PhysicalObject("test2");
	Test.BaseShader = 1;
	Test.BaseTexture = square_texture;
	Test.Col = Collider(CLDR_Point);
	Test.Color = glm::vec4(0,0,1,1);
	Test.Layer = 100;
	Test.Render = false;
	Scene.push_back(Test);
	
	for (int i = 1; i < 10; i++) {
		PhysicalObject Test2 = PhysicalObject("test");
		Test2.BaseShader = 1;
		Test2.BaseTexture = special_texture;
		Test2.Col = Collider(CLDR_Custom);
		Test2.Col.SetPoints({
			glm::vec2(-1.0f,  1.0f), glm::vec2(1.0f,  1.0f),
			glm::vec2(0.5f, -1.0f), glm::vec2(-0.5f, -1.0f)
		});
		Test2.Position = glm::vec2(i-5,0);
		Test2.Size = glm::vec2(1,(float)i/5);
		Scene.push_back(Test2);
	}
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