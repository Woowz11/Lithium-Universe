#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>

#include <algorithm>
#include <vector>

#include <box2cpp/box2cpp.h>

#include "GlobalRender.h";
#include "GameObject.h";
#include "GameCamera.h";
#include "GameData.h";

/* ==== Сцена ==== */

/* Сцена */
std::vector<GameObject> Scene = {};

/* Создать мир для Box2D */
void InstallBox2D() {
	b2::World w(b2::World::Params{});
}

/* Создать сцену */
void CreateScene() {
	GameObject Test = GameObject("test", RO_Phys);
	Test.BaseShader = 1;
	Test.BaseTexture = 1;
	Scene.push_back(Test);
}

/* Указать позицию курсора */
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2) {
	
}

/* Обновить физику */
std::vector<GameObject>& UpdatePhysic() {
	for (int i = 0; i < Scene.size(); i++) {
		GameObject& OBJ = Scene[i];
		if (OBJ.Active) {
			OBJ.AddRotation(0.0001f);
		}
	}

	return Scene;
}

/* Установить физику */
void InstallPhysic() {
	InstallBox2D();
	CreateScene();
}