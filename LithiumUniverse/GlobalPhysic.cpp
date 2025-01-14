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

/* Позиция мыши в мире */
glm::vec2 PhysicalMousePosition = glm::vec2(0, 0);
/* Название объекта мыши */
std::string MouseDetectorName = "GAME: MouseObject";
/* Айди объекта обозначающего мышь */
int MouseDetectorObject = -1;

/* Мышь наведена на этот объект */
RenderedObject* MouseOnThisObject = nullptr;

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

/* Проверка коллизии: Точка с кастомной (Функция) */
bool DT_PointToCustom_Func(glm::vec2 P, std::vector<glm::vec2> Points) {
	bool Result = false;

	int size = Points.size();
	for (int i = 0, j = size - 1; i < size; j = i++) {
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

/* Проверка коллизии: Точка с кастомной */
bool DT_PointToCustom(RenderedObject Point, RenderedObject Custom) {
	return DT_PointToCustom_Func(Point.Position, Custom.Col.GetPhysicalPoints(Custom.Position, Custom.Size, Custom.Orientation));
}

/* Проверка коллизии: Линия с кастомной (Функция) */
bool DT_LineToCustom_Func(glm::vec2 StartPos, glm::vec2 EndPos, std::vector<glm::vec2> Points) {
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

/* Проверка коллизии: Линия с кастомной */
bool DT_LineToCustom(RenderedObject Line, RenderedObject Custom) {
	return DT_LineToCustom_Func(Line.GetLineStartPosition(), Line.GetLineEndPosition(), Custom.Col.GetPhysicalPoints(Custom.Position, Custom.Size, Custom.Orientation));
}

/* Проверка коллизии: Кастомной с кастомной */
bool DT_CustomToCustom(RenderedObject Custom1, RenderedObject Custom2) {
	std::vector<glm::vec2> Points1 = Custom1.Col.GetPhysicalPoints(Custom1.Position, Custom1.Size, Custom1.Orientation);
	std::vector<glm::vec2> Points2 = Custom2.Col.GetPhysicalPoints(Custom2.Position, Custom2.Size, Custom2.Orientation);

	int size = Points1.size();
	for (int i = 0, j = size - 1; i < size; j = i++) {
		glm::vec2 VC = Points1[i];
		glm::vec2 VN = Points1[j];

		bool Hit = DT_LineToCustom_Func(VC, VN, Points2);
		if (Hit) { return true; }

		Hit = DT_PointToCustom_Func(Points2[0], Points1);
		if (Hit) { return true; }
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

	if (COL1.Type == CLDR_Custom && COL2.Type == CLDR_Custom) {
		return DT_CustomToCustom(OBJ2, OBJ1);
	}

	return false;
}

/* Мышку навели на объект */
void MouseOverOnObject() {
	MouseOnThisObject->Color = glm::vec4(1, 0, 0, 1);
}

/* Мышку убрали с объекта */
void MouseUnoverOnObject() {
	MouseOnThisObject->Color = glm::vec4(1, 1, 1, 1);
}

/* Выполнить физику для объекта */
RenderedObject* MouseOnThisObject_Result = nullptr;
void Physic(RenderedObject& OBJ, std::vector<RenderedObject>& Scene) {
	if (OBJ.Name == MouseDetectorName) {
		OBJ.SetPosition(PhysicalMousePosition);
	}

	if (OBJ.Selectable && OBJ.Render) {
		if (ObjectCollide(OBJ, Scene[MouseDetectorObject])) {
			if (MouseOnThisObject_Result == nullptr || (OBJ.Layer >= MouseOnThisObject_Result->Layer)) {
				MouseOnThisObject_Result = &OBJ;
			}
		}

		OBJ.AddRotation(dt * 30);
	}
}

/* Обрабатываеться после физики */
void AfterPhysic() {
	if (MouseOnThisObject == nullptr && MouseOnThisObject_Result != nullptr) {
		MouseOnThisObject = MouseOnThisObject_Result;
		MouseOverOnObject();
	}

	if (MouseOnThisObject != nullptr && MouseOnThisObject_Result == nullptr) {
		MouseUnoverOnObject();
		MouseOnThisObject = nullptr;
	}

	if (MouseOnThisObject != nullptr && MouseOnThisObject_Result != nullptr) {
		if (MouseOnThisObject->GetID() != MouseOnThisObject_Result->GetID()) {
			MouseUnoverOnObject();
			MouseOnThisObject = MouseOnThisObject_Result;
			MouseOverOnObject();
		}
	}

	MouseOnThisObject_Result = nullptr;
}

/* Создать сцену */
void CreateScene(std::vector<RenderedObject>& Scene) {
	/* ==== Создание мыши ==== */
	MouseDetectorObject = 0;
	RenderedObject MouseOBJ = RenderedObject(MouseDetectorName);
	MouseOBJ.Col = Collider(CLDR_Point);
	MouseOBJ.Render = false;
	Scene.push_back(MouseOBJ);

	int square_texture = 1;
	int special_texture = 2;
	int circle_texture = 3;
	int cable_texture = 4;

	for (int i = 1; i < 10; i++) {
		PhysicalObject Test2 = PhysicalObject("test");
		Test2.BaseShader = 1;
		Test2.BaseTexture = special_texture;
		Test2.Col.SetPoints({
			glm::vec2(-1.0f,  1.0f), glm::vec2(1.0f,  1.0f),
			glm::vec2(0.5f, -1.0f), glm::vec2(-0.5f, -1.0f)
			});
		Test2.Position = glm::vec2(i - 5, 0);
		Test2.Size = glm::vec2(1, (float)i / 5);
		Scene.push_back(Test2);
	}

	for (int i = 1; i < 50; i++) {
		PhysicalObject Test2 = PhysicalObject("test2");
		Test2.BaseShader = 1;
		Test2.BaseTexture = special_texture;
		Test2.Col.SetPoints({
			glm::vec2(-1.0f,  1.0f), glm::vec2(1.0f,  1.0f),
			glm::vec2(0.5f, -1.0f), glm::vec2(-0.5f, -1.0f)
		});
		Test2.Position = glm::vec2(0, (float)(i + 2)/6);
		Test2.Size = glm::vec2((float)i / 25, 1);
		Test2.Layer = 1.0f / i;
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
	AfterPhysic();
}