#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>

#include <algorithm>
#include <vector>

#include "GlobalRender.h";
#include "GameObject.h";
#include "GameCamera.h";
#include "HitResult.h";
#include "GameData.h";
#include "Collider.h";

/* Позиция мыши в мире */
glm::vec2 PhysicalMousePosition = glm::vec2(0, 0);
/* Название объекта мыши */
std::string MouseDetectorName = "GAME: MouseObject";
/* Айди объекта обозначающего мышь */
int MouseDetectorObject = -1;

/* Мышь наведена на этот объект */
GameObject* MouseOnThisObject = nullptr;

/* ==== Сцена ==== */

/* Сцена */
std::vector<GameObject> Scene = {};

/* Гравитация на сцене */
glm::vec2 Gravity = glm::vec2(0, -10);

/* Сопротивление воздуха */
float AirResistance = 1;

/* Скорость симуляции */
float SimulationSpeed = 1;

/* Установить время физики */
void SetPhysicTime(float npt) {
	SimulationSpeed = npt;
}

float dt = 0;
float pdt = 0;
float t = 0;
void UpdateDeltaTime_PHYSIC(float DT, float Time_) {
	dt = DT;
	t = Time_;

	pdt = dt * SimulationSpeed;
}


/* ==== Детект коллизий ==== */

class DT_Hit_Result {
public:
	bool Hit;
	glm::vec2 Pos;
	glm::vec2 Normal;

	DT_Hit_Result(glm::vec2 p, glm::vec2 n) {
		Hit = true;
		Pos = p;
		Normal = n;
	}
	DT_Hit_Result() {
		Hit = false;
		Pos = glm::vec2(0, 0);
		Normal = glm::vec2(0, 0);
	}
};

/* Проверка коллизии: Точка с точкой */
HitResult DC_PointToPoint(GameObject Point1, GameObject Point2) {
	return (Point1.Position.x == Point2.Position.x && Point1.Position.y == Point2.Position.y) ? HitResult(&Point2, Point1.Position, glm::vec2(0,0)) : HitResult();
}

/* Проверка коллизии: Точка с линией (Функция) */
bool DT_PointToLine_Func(glm::vec2 P, glm::vec2 SP, glm::vec2 EP) {
	float Accurate = 0.0001f;

	float Dist1 = glm::distance(P, SP);
	float Dist2 = glm::distance(P, EP);

	float LineLength = glm::distance(SP, EP);

	return (Dist1 + Dist2 >= LineLength - Accurate && Dist1 + Dist2 <= LineLength + Accurate) ? true : false;
}

/* Проверка коллизии: Точка с линией */
HitResult DT_PointToLine(GameObject Point, GameObject Line) {
	glm::vec2 LineVec = Line.GetLineEndPosition() - Line.GetLineStartPosition();
	return DT_PointToLine_Func(Point.Position, Line.GetLineStartPosition(), Line.GetLineEndPosition()) ? HitResult(&Line, Point.Position, glm::normalize(glm::vec2(-LineVec.y, LineVec.x))) : HitResult();
}

/* Проверка коллизии: Линия с линией (Функция) */
enum DT_LineToLine_HitType {
	DLTLHT_Perpendicular = 0,       /* Вернуть нормаль перпендикулярной второй линии                   */
	DLTLHT_PerpendicularInvert = 1, /* Вернуть нормаль перпендикулярной второй линии (инвертированный) */
	DLTLHT_Self = 2,                /* Вернуть нормаль паралельной первой линии                        */
	DLTLHT_Ricochet = 3,            /* Вернуть нормаль отскакивающую от линии                          */
	DLTLHT_Flow = 4,                /* Вернуть нормаль плавующую, будто в объекте                      */
	DLTLHT_FlowInvert = 5           /* Вернуть нормаль плавующую, будто в объекте (инвертированный)    */
};
DT_Hit_Result DT_LineToLine_Func(glm::vec2 Start1Pos, glm::vec2 End1Pos, glm::vec2 Start2Pos, glm::vec2 End2Pos, DT_LineToLine_HitType NormalType, glm::vec4 Sizes, glm::vec2 Rotations) {
	glm::vec2 DirA = End1Pos - Start1Pos;
	glm::vec2 DirB = End2Pos - Start2Pos;

	float D = DirA.x * DirB.y - DirA.y * DirB.x;

	glm::vec2 Dir = Start2Pos - Start1Pos;

	float Uu = (Dir.x * DirA.y - Dir.y * DirA.x) / D;
	float Ut = (Dir.x * DirB.y - Dir.y * DirB.x) / D;

	if (Ut >= 0 && Ut <= 1.0f && Uu >= 0 && Uu <= 1.0f) {
		glm::vec2 Normal;
		switch (NormalType) {
		case DLTLHT_Self:
			Normal = glm::normalize(-DirB);
			break;
		case DLTLHT_Ricochet:
			Normal = glm::normalize(-(-DirB - 2.0f * glm::dot(-DirB, glm::normalize(glm::vec2(-DirA.y, DirA.x))) * glm::normalize(glm::vec2(-DirA.y, DirA.x))));
			break;
		case DLTLHT_PerpendicularInvert:
			Normal = glm::normalize(-glm::vec2(-DirA.y, DirA.x));
			break;
		case DLTLHT_Flow:
			glm::vec2 RotatedDirA = glm::rotate(DirA, glm::radians(Rotations.x));
			glm::vec2 RotatedDirB = glm::rotate(DirB, glm::radians(Rotations.y));

			glm::vec2 TA = glm::normalize(glm::vec2(-RotatedDirA.y, RotatedDirA.x));
			glm::vec2 TB = glm::normalize(glm::vec2(-RotatedDirB.y, RotatedDirB.x));

			glm::vec2 ScaledA = TA * glm::vec2(Sizes.x, Sizes.y);
			glm::vec2 ScaledB = TB * glm::vec2(Sizes.w, Sizes.z);

			if (std::abs(ScaledA.x) + std::abs(ScaledB.x) > std::abs(ScaledA.y) + std::abs(ScaledB.y)) {
				Normal = glm::vec2(TA.x + TB.x > 0 ? 1.0f : -1.0f, 0.0f);
			}
			else {
				Normal = glm::vec2(0.0f, TA.y + TB.y > 0 ? 1.0f : -1.0f);
			}
			break;
		case DLTLHT_FlowInvert:
			glm::vec2 RotatedDirAi = glm::rotate(DirA, glm::radians(Rotations.x));
			glm::vec2 RotatedDirBi = glm::rotate(DirB, glm::radians(Rotations.y));

			glm::vec2 TAi = glm::normalize(glm::vec2(-RotatedDirAi.y, RotatedDirAi.x));
			glm::vec2 TBi = glm::normalize(glm::vec2(-RotatedDirBi.y, RotatedDirBi.x));

			glm::vec2 ScaledAi = TAi * glm::vec2(Sizes.x, Sizes.y);
			glm::vec2 ScaledBi = TBi * glm::vec2(Sizes.w, Sizes.z);

			if (std::abs(ScaledAi.x) + std::abs(ScaledBi.x) > std::abs(ScaledAi.y) + std::abs(ScaledBi.y)) {
				Normal = -glm::vec2(TAi.x + TBi.x > 0 ? 1.0f : -1.0f, 0.0f);
			}
			else {
				Normal = -glm::vec2(0.0f, TAi.y + TBi.y > 0 ? 1.0f : -1.0f);
			}
			break;
		default:
			Normal = glm::normalize(glm::vec2(-DirA.y, DirA.x));
			break;
		}
		return DT_Hit_Result(Start1Pos + Ut * DirA, Normal);
	}

	return DT_Hit_Result();
}
DT_Hit_Result DT_LineToLine_Func(glm::vec2 Start1Pos, glm::vec2 End1Pos, glm::vec2 Start2Pos, glm::vec2 End2Pos, DT_LineToLine_HitType NormalType) {
	return DT_LineToLine_Func(Start1Pos, End1Pos, Start2Pos, End2Pos, NormalType, glm::vec4(1,1,1,1), glm::vec2(0,0));
}

/* Проверка коллизии: Линия с линией */
HitResult DT_LineToLine(GameObject Line1, GameObject Line2) {
	DT_Hit_Result R = DT_LineToLine_Func(Line1.GetLineStartPosition(), Line1.GetLineEndPosition(), Line2.GetLineStartPosition(), Line2.GetLineEndPosition(), DLTLHT_Self);
	return R.Hit ? HitResult(&Line2, R.Pos, R.Normal) : HitResult();
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
HitResult DT_PointToCustom(GameObject Point, GameObject Custom) {
	return DT_PointToCustom_Func(Point.Position, Custom.Col.GetPhysicalPoints(Custom.Position, Custom.Size, Custom.Orientation)) ? HitResult(&Custom, Point.Position, glm::vec2(0, 0)) : HitResult();
}

/* Проверка коллизии: Линия с кастомной (Функция) */
std::vector<DT_Hit_Result> DT_LineToCustom_Func(glm::vec2 StartPos, glm::vec2 EndPos, std::vector<glm::vec2> Points, DT_LineToLine_HitType HitType , bool Sort, glm::vec4 Sizes, glm::vec2 Rotations) {
	std::vector<DT_Hit_Result> Result = {};
	
	int size = Points.size();
	for (int i = 0, j = size - 1; i < size; j = i++) {
		glm::vec2 VC = Points[i];
		glm::vec2 VN = Points[j];

		DT_Hit_Result R = DT_LineToLine_Func(StartPos, EndPos, VC, VN, HitType, Sizes, Rotations);
		if (R.Hit) {
			Result.push_back(R);
		}
	}

	if (Result.empty()) { return { DT_Hit_Result() }; }

	if (Sort) {
		std::sort(Result.begin(), Result.end(), [StartPos](const DT_Hit_Result& a, const DT_Hit_Result& b) {
			return glm::distance(StartPos, a.Pos) < glm::distance(StartPos, b.Pos);
		});
	}

	return Result;
}

/* Проверка коллизии: Линия с кастомной */
HitResult DT_LineToCustom(GameObject Line, GameObject Custom) {
	DT_Hit_Result R = DT_LineToCustom_Func(Line.GetLineStartPosition(), Line.GetLineEndPosition(), Custom.Col.GetPhysicalPoints(Custom.Position, Custom.Size, Custom.Orientation), DLTLHT_Self, true, glm::vec4(1,1, Custom.Size.x, Custom.Size.y), glm::vec2(0, Custom.Orientation))[0];
	return R.Hit ? HitResult(&Custom, R.Pos, R.Normal, glm::distance(R.Pos, Line.GetLineEndPosition())) : HitResult();
}

/* Проверка коллизии: Кастомной с кастомной */
HitResult DT_CustomToCustom(GameObject Custom1, GameObject Custom2) {
	std::vector<glm::vec2> Points1 = Custom1.Col.GetPhysicalPoints(Custom1.Position, Custom1.Size, Custom1.Orientation);
	std::vector<glm::vec2> Points2 = Custom2.Col.GetPhysicalPoints(Custom2.Position, Custom2.Size, Custom2.Orientation);

	float MinDistance = std::numeric_limits<float>::max();
	glm::vec2 ClosestPoint(0.0f);
	glm::vec2 Normal(0.0f);
	bool FoundCollision = false;

	int size1 = Points1.size();
	for (int i = 0, j = size1 - 1; i < size1; j = i++) {
		glm::vec2 VC1 = Points1[i];
		glm::vec2 VN1 = Points1[j];

		std::vector<DT_Hit_Result> Hits = DT_LineToCustom_Func(VC1, VN1, Points2, /*DLTLHT_FlowInvert*/ DLTLHT_PerpendicularInvert, false, glm::vec4(Custom1.Size.x, Custom1.Size.y, Custom2.Size.x, Custom2.Size.y), glm::vec2(Custom1.Orientation, Custom2.Orientation));
		for (const auto& Hit : Hits) {
			if (Hit.Hit) {
				FoundCollision = true;
				float Distance = glm::distance(Custom1.Position, Hit.Pos);
				if (Distance < MinDistance) {
					MinDistance = Distance;
					ClosestPoint = Hit.Pos;
					Normal = Hit.Normal;
				}
			}
		}
	}

	if (!FoundCollision && DT_PointToCustom_Func(Custom1.Position, Points2)) {
		FoundCollision = true;
		ClosestPoint = Custom1.Position;
		MinDistance = glm::distance(Custom1.Position, Custom2.Position);
		Normal = glm::normalize(Custom2.Position - Custom1.Position);
	}

	if (FoundCollision) {
		return HitResult(&Custom2, ClosestPoint, Normal, MinDistance);
	}

	return HitResult();
}

/* ==== Физические действия ==== */

/* Объекты прикосаются с друг другом? */
HitResult ObjectCollide(GameObject OBJ1, GameObject OBJ2) {
	Collider COL1 = OBJ1.Col;
	Collider COL2 = OBJ2.Col;

	if (COL1.Type == CLDR_None || COL2.Type == CLDR_None) {
		return HitResult();
	}

	if (COL1.Type == CLDR_Custom && COL2.Type == CLDR_Custom) {
		return DT_CustomToCustom(OBJ2, OBJ1);
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

	if (COL1.Type == CLDR_Line && COL2.Type == CLDR_Line) {
		return DT_LineToLine(OBJ1, OBJ2);
	}

	if (COL1.Type == CLDR_Point && COL2.Type == CLDR_Line) {
		return DT_PointToLine(OBJ1, OBJ2);
	}
	if (COL2.Type == CLDR_Point && COL1.Type == CLDR_Line) {
		return DT_PointToLine(OBJ2, OBJ1);
	}

	if (COL1.Type == CLDR_Point && COL2.Type == CLDR_Point) {
		return DC_PointToPoint(OBJ1, OBJ2);
	}

	return HitResult();
}

/* ==== Физика ==== */

/* Применить гравитацию */
void ApplyGravity(GameObject& OBJ) {
	/*glm::vec2 GravityForce = Gravity * OBJ.Mass;

	glm::vec2 AirResistanceForce = -(AirResistance * OBJ.Velocity * glm::length(OBJ.Velocity));

	glm::vec2 NetForce = GravityForce + AirResistanceForce;

	glm::vec2 Acceleration = NetForce / OBJ.Mass;

	OBJ.Velocity += Acceleration * pdt;*/

	OBJ.Velocity += Gravity * pdt;
}

void ResolveCollision(GameObject& OBJ, GameObject& OBJ2) {
	HitResult Hit = ObjectCollide(OBJ, OBJ2);
	if (!Hit.Hit) { return; }

	Scene[1].Position = Hit.HitPosition;
	Scene[2].SetLineStartPosition(Hit.HitPosition);
	Scene[2].SetLineEndPosition(Hit.HitPosition + Hit.Normal);

	glm::vec2 RVelocity = OBJ2.GetVelocity() - OBJ.GetVelocity();
	float VAN = glm::dot(RVelocity, Hit.Normal);

	float E = std::min(OBJ.Restitution, OBJ2.Restitution);

	float J = -(1 - E) * VAN;
	J /= (1 / OBJ.Mass) + (1 / OBJ2.Mass);

	glm::vec2 I = J * Hit.Normal;

	OBJ.Impulse(-(I / OBJ.Mass));
	OBJ2.Impulse(I / OBJ2.Mass);

	const float penetrationCorrection = 0.001f;

	glm::vec2 correction = penetrationCorrection * Hit.Depth * Hit.Normal;
	OBJ.Position += correction * (1 / OBJ.Mass) / ((1 / OBJ.Mass) + (1 / OBJ.Mass));
	if (!OBJ2.Static) {
		OBJ2.Position -= correction * (1 / OBJ2.Mass) / ((1 / OBJ2.Mass) + (1 / OBJ2.Mass));
	}
}

void ApplyVelocities(GameObject& OBJ) {
	OBJ.Position += OBJ.Velocity * pdt;
}

/* Обработка физики */
void WorkPhysic(GameObject& OBJ, int i) {
	ApplyGravity(OBJ);
	for (int j = 0; j < Scene.size(); j++) {
		if (i != j) {
			GameObject& OBJ2 = Scene[j];
			if (OBJ2.Type == RO_Phys) {
				ResolveCollision(OBJ, OBJ2);
			}
		}
	}
	ApplyVelocities(OBJ);
}

/* Мышку навели на объект */
void MouseOverOnObject() {
	//MouseOnThisObject->Color = glm::vec4(1, 0, 0, 1);
}

/* Мышку убрали с объекта */
void MouseUnoverOnObject() {
	//MouseOnThisObject->Color = glm::vec4(1, 1, 1, 1);
}

void RemoveGameObject(int i) {
	if (i >= 0 && i < Scene.size()) {
		Scene.erase(Scene.begin() + i);
	}
	else {
		Error("SCENE", "Cannot delete GameObject because its i is outside the scene! RemoveGameObject(" + std::to_string(i) + ");");
	}
}

void CreateObjectTest() {
	GameObject Test = GameObject("test", RO_Phys);
	Test.BaseShader = 1;
	Test.BaseTexture = 1;
	Test.Position = PhysicalMousePosition;
	Scene.push_back(Test);
}

/* Выполнить физику для объекта */
GameObject* MouseOnThisObject_Result = nullptr;
void Physic(GameObject& OBJ, int i) {
	if (OBJ.Name == MouseDetectorName) {
		OBJ.SetPosition(PhysicalMousePosition);
	}

	if (OBJ.Selectable && OBJ.Render) {
		HitResult Hit = ObjectCollide(OBJ, Scene[MouseDetectorObject]);
		if (Hit.Hit) {
			if (MouseOnThisObject_Result == nullptr || (OBJ.Layer >= MouseOnThisObject_Result->Layer)) {
				MouseOnThisObject_Result = &OBJ;
			}
		}
	}

	/* Обновление физики у объекта */
	if (OBJ.Type == RO_Phys && SimulationSpeed != 0) {
		if (!OBJ.Static) {
			WorkPhysic(OBJ, i);
			if (OBJ.Position.y <= -300) {
				RemoveGameObject(i);
			}
		}
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
void CreateScene() {

	/* ==== Создание мыши ==== */

	MouseDetectorObject = 0;
	GameObject MouseOBJ = GameObject(MouseDetectorName);
	MouseOBJ.Col = Collider(CLDR_Point);
	MouseOBJ.Render = false;
	Scene.push_back(MouseOBJ);

	int square_texture = 1;
	int special_texture = 2;
	int circle_texture = 3;
	int cable_texture = 4;

	GameObject TestHIT = GameObject("testhit");
	TestHIT.BaseShader = 1;
	TestHIT.BaseTexture = circle_texture;
	TestHIT.Size = glm::vec2(0.1f, 0.1f);
	TestHIT.Position = glm::vec2(-100, -100);
	TestHIT.Color = glm::vec4(0, 1, 0, 1);
	TestHIT.Layer = 1000;
	Scene.push_back(TestHIT);

	GameObject Test21 = GameObject("normal");
	Test21.MakeItLine(glm::vec2(0, 0), glm::vec2(0, 0), 0.1f);
	Test21.BaseTexture = 4;
	Test21.Color = glm::vec4(1, 0, 0, 1);
	Test21.Layer = 900;
	Scene.push_back(Test21);

	GameObject Test = GameObject("test", RO_Phys);
	Test.BaseShader = 1;
	Test.BaseTexture = square_texture;
	Test.Position = glm::vec2(0, 3);
	Scene.push_back(Test);

	GameObject Test2 = GameObject("test2", RO_Phys);
	Test2.BaseShader = 1;
	Test2.BaseTexture = square_texture;
	Test2.Size = glm::vec2(1000, 1);
	Test2.Color = glm::vec4(0.25f, 0.25f, 0.25f, 1);
	Test2.Position = glm::vec2(0, -3);
	Test2.SetRotation(15);
	Test2.Static = true;
	Test2.Mass = 100;
	Scene.push_back(Test2);
}

/* Указать позицию курсора */
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2) {
	PhysicalMousePosition = ScreenPositionToWorld(Pos2);
}

/* Обновить физику */
std::vector<GameObject>& UpdatePhysic() {
	for (int i = 0; i < Scene.size(); i++) {
		GameObject& OBJ = Scene[i];
		if (OBJ.Active) {
			Physic(OBJ, i);
		}
	}
	AfterPhysic();

	return Scene;
}

/* Установить физику */
void InstallPhysic() {
	CreateScene();
}