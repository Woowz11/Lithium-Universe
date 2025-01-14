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
	std::vector<glm::vec2> Points = Custom.Col.GetPhysicalPoints(Custom.Position, Custom.Size, Custom.Orientation);

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

/* Проверка коллизии: Линия с кастомной */
bool DT_LineToCustom(RenderedObject Line, RenderedObject Custom) {
	glm::vec2 StartPos = Line.GetLineStartPosition();
	glm::vec2 EndPos = Line.GetLineEndPosition();
	std::vector<glm::vec2> Points = Custom.Col.GetPhysicalPoints(Custom.Position, Custom.Size, Custom.Orientation);

	int size = Points.size();
	for (int i = 0, j = size - 1; i < size; j = i++) {
		glm::vec2 VC = Points[i];
		glm::vec2 VN = Points[j];

		if (DT_LineToLine_Func(StartPos, EndPos, VC, VN)) {
			return true;
		}
	}
	return false;
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

	if (COL1.Type == CLDR_Point && COL2.Type == CLDR_Line) {
		return DT_PointToLine(OBJ1, OBJ2);
	}
	if (COL2.Type == CLDR_Point && COL1.Type == CLDR_Line) {
		return DT_PointToLine(OBJ2, OBJ1);
	}

	if (COL1.Type == CLDR_Line && COL2.Type == CLDR_Line) {
		return DT_LineToLine(OBJ1, OBJ2);
	}

	if (COL1.Type == CLDR_Point && COL2.Type == CLDR_Custom) {
		return DT_PointToCustom(OBJ1, OBJ2);
	}
	if (COL2.Type == CLDR_Point && COL1.Type == CLDR_Custom) {
		return DT_PointToCustom(OBJ2, OBJ1);
	}

	if (COL1.Type == CLDR_Line && COL2.Type == CLDR_Custom) {
		return DT_LineToCustom(OBJ1, OBJ2);
	}
	if (COL2.Type == CLDR_Line && COL1.Type == CLDR_Custom) {
		return DT_LineToCustom(OBJ2, OBJ1);
	}

	return false;
}

/* Выполнить физику для объекта */
void Physic(RenderedObject& OBJ, std::vector<RenderedObject>& Scene) {
	if (OBJ.Name == "test2") {
		//OBJ.SetPosition(PhysicalMousePosition);
		OBJ.SetLineEndPosition(PhysicalMousePosition);
	}
	else {
		bool collide = ObjectCollide(OBJ, Scene[0]);
		OBJ.Color = (collide ? glm::vec4(0, 1, 0, 1) : glm::vec4(1, 0, 0, 1));
		OBJ.AddRotation(dt*30);
	}
}

/* Создать сцену */
void CreateScene(std::vector<RenderedObject>& Scene) {
	int square_texture = 1;
	int special_texture = 2;
	int circle_texture = 3;
	int cable_texture = 4;

	/*PhysicalObject Test = PhysicalObject("test2");
	Test.BaseShader = 1;
	Test.BaseTexture = square_texture;
	Test.Col = Collider(CLDR_Point);
	Test.Color = glm::vec4(0,0,1,1);
	Test.Layer = 100;
	Test.Render = false;
	Scene.push_back(Test);*/

	PhysicalObject Test2 = PhysicalObject("test2");
	Test2.MakeItLine(glm::vec2(0, -10), glm::vec2(0, 0), 0.1f);
	Test2.Col = Collider(CLDR_Line);
	Test2.BaseTexture = 4;
	Test2.Color = glm::vec4(0, 0, 1, 1);
	Test2.Layer = 100;
	Scene.push_back(Test2);

	for (int i = 1; i < 10; i++) {
		PhysicalObject Test2 = PhysicalObject("test");
		Test2.BaseShader = 1;
		Test2.BaseTexture = special_texture;
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