#include <vector>

#include "GlobalRender.h";
#include "RenderedObject.h";
#include "GameCamera.h";
#include "GameData.h";

float dt = 0;
void UpdateDeltaTime_PHYSIC(float DT) {
	dt = DT;
}

/* Установить физику */
void InstallPhysic() {

}

/* ==== Физические действия ==== */

/* Выполнить физику для объекта */
void Physic(RenderedObject& OBJ) {
	//OBJ.AddRotation(dt * 20);
	//OBJ.Size = glm::vec2((OBJ.Orientation.z - 180) / 90, (OBJ.Orientation.z - 180) / 90);
	if (OBJ.Name == "test2") {
		OBJ.SetPosition(Camera->Position);
	}
}

/* Создать сцену */
void CreateScene(std::vector<RenderedObject>& Scene) {
	RenderedObject Test = RenderedObject("test");
	Test.BaseShader = 1;
	Test.BaseTexture = 1;
	Test.Orientation = glm::vec3(0, 0, 45);
	Test.Size = glm::vec2(0.5f, 3);
	Scene.push_back(Test);

	RenderedObject Test2 = RenderedObject("test2");
	Test2.BaseShader = 1;
	Test2.BaseTexture = 1;
	Test2.Color = glm::vec4(1,0.5f,0.5f,1);
	Scene.push_back(Test2);
}

/* Обновить физику */
void UpdatePhysic(std::vector<RenderedObject>& Scene) {
	for (RenderedObject& OBJ : Scene) {
		Physic(OBJ);
	}
}