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

/* ==== Сцена ==== */

/* Сцена */
std::vector<GameObject> Scene = {};

/* Создать сцену */
void CreateScene() {

}

/* Указать позицию курсора */
void UpdateMousePhysic(glm::vec2 Pos, glm::vec2 Pos2) {
	
}

/* Обновить физику */
std::vector<GameObject>& UpdatePhysic() {
	for (int i = 0; i < Scene.size(); i++) {
		GameObject& OBJ = Scene[i];
		if (OBJ.Active) {
			
		}
	}

	return Scene;
}

/* Установить физику */
void InstallPhysic() {
	CreateScene();
}