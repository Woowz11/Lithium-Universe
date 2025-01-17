#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>

#include <box2d/box2d.h>

#include "GameObjectActions.h";
#include "GlobalRender.h";
#include "GlobalPhysic.h";
#include "GameInstalls.h";
#include "GameCamera.h";
#include "GameData.h";

/* Скрипт отвечающий за управление игроком */

/* Объект который держит игрок мышкой */
int PickupedObject = -1;

/* Джоинт мыши */
bool HasMouseJoint = false;
b2JointId MouseJoint = b2_nullJointId;

/* Обновить джоин мыши */
void UpdateMouseJoint() {
	if (HasMouseJoint) {
		b2DestroyJoint(MouseJoint);
		HasMouseJoint = false;
	}

	if (PickupedObject != -1) {
		b2MouseJointDef J = b2DefaultMouseJointDef();
		b2BodyId Body = GetBody(PickupedObject);
		J.bodyIdA = GetBody(MouseObjectConnector);
		J.bodyIdB = Body;
		J.target = Vec2ToBVec2(MouseWorldPosition);
		J.hertz = 5.0f;
		J.dampingRatio = 0.7f;
		J.maxForce = 10000.f * b2Body_GetMass(Body);
		//J.localAnchorA = b2Vec2(0, 0);
		//J.localAnchorB = b2Body_GetLocalPoint(Body, Vec2ToBVec2(MouseWorldPosition));
		MouseJoint = b2CreateMouseJoint(World, &J);

		b2Body_SetAwake(Body, true);

		HasMouseJoint = true;
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

/* ==== Основа ==== */

/* Курсор двигается */
void MouseMove(glm::vec2 Pos, glm::vec2 Pos2) {
	if (HasMouseJoint) {
		b2MouseJoint_SetTarget(MouseJoint, Vec2ToBVec2(MouseWorldPosition));
	}
}

/* Кнопка на мыши нажата */
void MouseClick(int KEY, int ACTION) {
	if (KEY == GLFW_MOUSE_BUTTON_RIGHT && ACTION == GLFW_PRESS) {
		CreateTestObject(0);
	}

	if (KEY == GLFW_MOUSE_BUTTON_LEFT) {
		if (ACTION == GLFW_PRESS) {
			MousePickupGO(GetMouseObject());
		}
		if (ACTION == GLFW_RELEASE) {
			MouseDropGO();
		}
	}
}

/* Управление клавиатурой */
bool SpacePressed = false;
void ControlsKeyboard(int KEY, int ACTION) {
	if (KEY == GLFW_KEY_SPACE && ACTION == GLFW_PRESS) {
		SpacePressed = !SpacePressed;
		SetSimulationSpeed(SpacePressed ? 0 : 1);
	}

	if (KEY == GLFW_KEY_Q && ACTION == GLFW_PRESS) {
		SpacePressed = false;
		SetSimulationSpeed(0.1f);
	}

	if (KEY == GLFW_KEY_1 && ACTION == GLFW_PRESS) {
		CreateTestObject(0);
	}

	if (KEY == GLFW_KEY_2 && ACTION == GLFW_PRESS) {
		CreateTestObject(1);
	}
}

/* Управление клавиатурой (Каждый тик) */
void ControlsKeyboardTick() {
	if (KeyPressed(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		ExitGame();
	}

	if (KeyPressed(GLFW_KEY_HOME) == GLFW_PRESS) {
		Camera->SetCameraRotation(0);
		Camera->SetCameraZoom(1);
		Camera->SetCameraPosition(0, 0);
	}

	bool W = KeyPressed(GLFW_KEY_W) == GLFW_PRESS;
	bool S = KeyPressed(GLFW_KEY_S) == GLFW_PRESS;
	bool D = KeyPressed(GLFW_KEY_D) == GLFW_PRESS;
	bool A = KeyPressed(GLFW_KEY_A) == GLFW_PRESS;

	bool SHIFT   = KeyPressed(GLFW_KEY_LEFT_SHIFT  ) == GLFW_PRESS;
	bool CONTROL = KeyPressed(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

	float speed = (SHIFT ? 3 : (CONTROL ? 0.3f : 1));

	if (W && !S) {
		Camera->MoveCamera(0, speed);
	}
	if (S && !W) {
		Camera->MoveCamera(0, -speed);
	}

	if (D && !A) {
		Camera->MoveCamera(speed, 0);
	}
	if (A && !D) {
		Camera->MoveCamera(-speed, 0);
	}

	bool PLUS  = KeyPressed(GLFW_KEY_EQUAL) == GLFW_PRESS;
	bool MINUS = KeyPressed(GLFW_KEY_MINUS) == GLFW_PRESS;

	if (PLUS && !MINUS) {
		Camera->MoveCameraZoom(speed);
	}
	if (MINUS && !PLUS) {
		Camera->MoveCameraZoom(-speed);
	}

	bool RIGHT = KeyPressed(GLFW_KEY_RIGHT) == GLFW_PRESS;
	bool LEFT  = KeyPressed(GLFW_KEY_LEFT ) == GLFW_PRESS;

	if (RIGHT && !LEFT) {
		Camera->MoveCameraRotation(speed);
	}
	if (LEFT && !RIGHT) {
		Camera->MoveCameraRotation(-speed);
	}
}

/* Управление */
void Controls() {
	ControlsKeyboardTick();
}