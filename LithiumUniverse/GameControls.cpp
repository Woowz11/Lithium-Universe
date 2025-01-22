#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>

#include <box2d/box2d.h>

#include "GameObjectActions.h";
#include "GameControls.h";
#include "GlobalRender.h";
#include "GlobalPhysic.h";
#include "GameInstalls.h";
#include "GameCamera.h";
#include "GameData.h";
#include "GlobalUI.h";

/* Скрипт отвечающий за управление игроком */

/* Объект который держит игрок мышкой */
int PickupedObject = -1;

/* Джоинт мыши */
bool HasMouseJoint = false;
b2JointId MouseJoint = b2_nullJointId;

/* Обновить джоин мыши */
void UpdateMouseJoint() {
	if (HasMouseJoint) {
		if (b2Joint_IsValid(MouseJoint)) {
			b2DestroyJoint(MouseJoint);
		}
		HasMouseJoint = false;
	}

	if (PickupedObject != -1) {
		if (CheckOutSceneIndex(PickupedObject)) {
			PickupedObject = -1;
			UpdateMouseJoint();
			return;
		}

		GameObject& OBJ = GetGameObject(PickupedObject, "UpdateMouseJoint(); #PickupedObject#");

		if (OBJ.Deleted || !OBJ.Active) {
			PickupedObject = -1;
		}
		else {
			b2MouseJointDef J = b2DefaultMouseJointDef();
			b2BodyId Body = GetBody(OBJ.BodyID);
			J.bodyIdA = GetBody(GetGameObject(MouseObjectConnector, "UpdateMouseJoint(); #MouseObjectConnector#").BodyID);
			J.bodyIdB = Body;
			J.target = Vec2ToBVec2(MouseWorldPosition);
			J.hertz = 5.0f;
			J.dampingRatio = 0.7f;
			J.maxForce = 1000000.f * b2Body_GetMass(Body);
			MouseJoint = b2CreateMouseJoint(World, &J);

			b2Body_SetAwake(Body, true);

			HasMouseJoint = true;
		}
	}
}

/* Поднять объект */
void MousePickupGO(int i) {
	if (i != PickupedObject) {
		PickupedObject = i;
		UpdateMouseJoint();
	}
}

/* Бросить объект */
void MouseDropGO() {
	PickupedObject = -1;
	UpdateMouseJoint();
}

/* Обработать клик по интерфейсу */
void ProcessUIClick(int i, bool Left) {
	GameObject& OBJ = GetGameObject(i, "ProcessUIClick(" + std::to_string(i) + "," + ToStringBool(Left) + ");");

	Button B = Buttons[OBJ.ButtonID];
	if (Left) {
		B.WhenLeftClick();
	}
	else {
		B.WhenRightClick();
	}
}

/* ==== Основа ==== */

bool SHIFT = false;
bool CONTROL = false;

bool CameraDrag = false;
glm::vec2 CameraDragOffset = glm::vec2(0, 0);

/* Курсор двигается */
void MouseMove() {
	if (HasMouseJoint) {
		if (b2Joint_IsValid(MouseJoint)) {
			b2MouseJoint_SetTarget(MouseJoint, Vec2ToBVec2(MouseWorldPosition));
		}
		else {
			MouseDropGO();
		}
	}

	if (CameraDrag) {
		glm::vec2 Dif = CameraDragOffset - MouseWorldPosition;
		Camera->MoveCamera(Dif.x, Dif.y, (1/Camera->Zoom) * DeltaTime * 10);
	}
}

/* Кнопка на мыши нажата */
void MouseClick(int KEY, int ACTION) {
	if (KEY == GLFW_MOUSE_BUTTON_RIGHT && ACTION == GLFW_PRESS) {
		CreateTestObject(1);
	}

	if (KEY == GLFW_MOUSE_BUTTON_LEFT) {
		if (ACTION == GLFW_PRESS) {
			if (MouseOnInterface) {
				ProcessUIClick(MouseUIObject, true);
			}
			else {
				MousePickupGO(MouseObject);
			}
		}
		if (ACTION == GLFW_RELEASE) {
			MouseDropGO();
		}
	}

	if (KEY == GLFW_MOUSE_BUTTON_RIGHT) {
		if (ACTION == GLFW_PRESS) {
			if (MouseOnInterface) {
				ProcessUIClick(MouseUIObject, false);
			}
		}
	}

	if (KEY == GLFW_MOUSE_BUTTON_MIDDLE) {
		if (ACTION == GLFW_PRESS) {
			CameraDrag = true;
			CameraDragOffset = MouseWorldPosition;
		}
		if (ACTION == GLFW_RELEASE) {
			CameraDrag = false;
		}
	}
}

/* Колёсико мышки двигается */
void MouseScroll(float scroll) {
	Camera->MoveCameraZoom(scroll * (SHIFT ? 3 : (CONTROL ? 0.3f : 1)) * 0.1f, 1);
}

/* Управление клавиатурой */
bool SpacePressed = false;
void ControlsKeyboard(int KEY, int ACTION) {
	if (KEY == GLFW_KEY_ESCAPE && ACTION == GLFW_PRESS) {
		if (CurrentScene == SCENE_Game) {
			SetScene(SCENE_MainMenu);
		}
		else {
			ExitGame();
		}
	}

	if (KEY == GLFW_KEY_HOME && ACTION == GLFW_PRESS) {
		Camera->ResetCamera();
	}

	if (KEY == GLFW_KEY_SPACE && ACTION == GLFW_PRESS) {
		SpacePressed = !SpacePressed;
		SetSimulationSpeed(SpacePressed ? 0 : 1);
	}

	if (KEY == GLFW_KEY_G && ACTION == GLFW_PRESS) {
		SpacePressed = false;
		SetSimulationSpeed(0.1f);
	}

	if (KEY == GLFW_KEY_Z && ACTION == GLFW_PRESS) {
		RemoveLastTestObject();
	}

	if (KEY == GLFW_KEY_1 && ACTION == GLFW_PRESS) {
		CreateTestObject(0);
	}

	if (KEY == GLFW_KEY_2 && ACTION == GLFW_PRESS) {
		CreateTestObject(1);
	}

	if (KEY == GLFW_KEY_3 && ACTION == GLFW_PRESS) {
		CreateTestObject(2);
	}

	if (KEY == GLFW_KEY_4 && ACTION == GLFW_PRESS) {
		CreateTestObject(3);
	}

	if (KEY == GLFW_KEY_5 && ACTION == GLFW_PRESS) {
		CreateTestObject(4);
	}

	if (KEY == GLFW_KEY_6 && ACTION == GLFW_PRESS) {
		CreateTestObject(5);
	}

	if (KEY == GLFW_KEY_7 && ACTION == GLFW_PRESS) {
		CreateTestObject(6);
	}

	if (KEY == GLFW_KEY_8 && ACTION == GLFW_PRESS) {
		CreateTestObject(7);
	}
}

/* Управление клавиатурой (Каждый тик) */
void ControlsKeyboardTick() {
	bool W = KeyPressed(GLFW_KEY_W) == GLFW_PRESS;
	bool S = KeyPressed(GLFW_KEY_S) == GLFW_PRESS;
	bool D = KeyPressed(GLFW_KEY_D) == GLFW_PRESS;
	bool A = KeyPressed(GLFW_KEY_A) == GLFW_PRESS;

	SHIFT   = KeyPressed(GLFW_KEY_LEFT_SHIFT  ) == GLFW_PRESS;
	CONTROL = KeyPressed(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

	float speed = (SHIFT ? 3 : (CONTROL ? 0.3f : 1));

	if (W && !S) {
		Camera->MoveCamera(0, speed, DeltaTime);
	}
	if (S && !W) {
		Camera->MoveCamera(0, -speed, DeltaTime);
	}

	if (D && !A) {
		Camera->MoveCamera(speed, 0, DeltaTime);
	}
	if (A && !D) {
		Camera->MoveCamera(-speed, 0, DeltaTime);
	}

	bool PLUS  = KeyPressed(GLFW_KEY_EQUAL) == GLFW_PRESS;
	bool MINUS = KeyPressed(GLFW_KEY_MINUS) == GLFW_PRESS;

	if (PLUS && !MINUS) {
		Camera->MoveCameraZoom(speed, DeltaTime);
	}
	if (MINUS && !PLUS) {
		Camera->MoveCameraZoom(-speed, DeltaTime);
	}

	bool RIGHT = KeyPressed(GLFW_KEY_RIGHT) == GLFW_PRESS;
	bool LEFT  = KeyPressed(GLFW_KEY_LEFT ) == GLFW_PRESS;

	if (RIGHT && !LEFT) {
		Camera->MoveCameraRotation(speed, DeltaTime);
	}
	if (LEFT && !RIGHT) {
		Camera->MoveCameraRotation(-speed, DeltaTime);
	}
}

/* Управление */
void Controls() {
	ControlsKeyboardTick();
}