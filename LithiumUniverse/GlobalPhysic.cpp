#include <vector>

#include "GlobalRender.h";
#include "RenderedObject.h";

float dt = 0;
void UpdateDeltaTime_PHYSIC(float DT) {
	dt = DT;
}

/* Установить физику */
void InstallPhysic() {

}

/* Выполнить физику для объекта */
void Physic(RenderedObject& OBJ) {
	OBJ.AddRotation(dt * 20);
	//OBJ.Size = glm::vec2((OBJ.Orientation.z - 180) / 90, (OBJ.Orientation.z - 180) / 90);
}

/* Обновить физику */
void UpdatePhysic(std::vector<RenderedObject>& Scene) {
	for (RenderedObject& OBJ : Scene) {
		Physic(OBJ);
	}
}