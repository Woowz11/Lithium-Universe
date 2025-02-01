#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>

#include <vector>

#include "GameObjectActions.h";
#include "GlobalPhysic.h";
#include "GlobalRender.h";
#include "GlobalUI.h";

/* Окно в фокусе? */
bool GameInFocus = true;

/* Мышка на интерфейсе */
bool MouseOnInterface = false;

/* Кнопки */
std::vector<Button> Buttons = {};

/* Объект на который наведена мышь (интерфейс) */
int MouseUIObject = -1;

/* Сделать объект интерфейсом (private) */
void MakeGameObjectUI__(int i) {
	GameObject& OBJ = GetGameObject(i, "MakeGameObjectUI__(" + std::to_string(i) + ");");
	OBJ.Type = RO_UI;
	SetGameObjectSelectable(i, false);
	OBJ.Resize = true;
	SetGameObjectLayer(i, 1000);
}

/* Обновление кнопки при наведении курсора на неё (private) */
void ButtonHover__(int i, bool Hover) {
	if (i >= 0) {
		GameObject& OBJ = GetGameObject(i, "ButtonHover__(" + std::to_string(i) + "," + ToStringBool(Hover) + ");");
		if (OBJ.ButtonID != -1) {
			if (Hover) {
				SetGameObjectTexture(i, GetResource("Base", "Textures/ButtonHover.png").ID);
			}
			else {
				SetGameObjectTexture(i, GetResource("Base", "Textures/Button.png").ID);
			}
		}
	}
}

/* Превратить объект в кнопку */
void MakeGameObjectButton(int i, std::function<void()> WhenLeftClick, std::function<void()> WhenRightClick) {
	GameObject& OBJ = GetGameObject(i, "MakeGameObjectButton(" + std::to_string(i) + ",?,?);");
	SetGameObjectTexture(i, GetResource("Base", "Textures/Button.png").ID);
	int Bi = Buttons.size();
	OBJ.ButtonID = Bi;
	Button B = Button(i,WhenLeftClick,WhenRightClick);
	Buttons.push_back(B);
}

/* Превратить объект в текст */
void MakeGameObjectText(int i, std::string Text) {
	GameObject& OBJ = GetGameObject(i, "MakeGameObjectText(" + std::to_string(i) + ",\"" + Text + "\");");
	OBJ.RenderType = RT_Text;
	SetGameObjectText(i, Text);
	SetGameObjectShader(i, GetResource("Base", "Shaders/Text.lu_shader").ID);
	SetGameObjectFont(i, GetResource("Base", "Fonts/Default.lu_font").ID);
	SetGameObjectTexture(i, GetResource("Base", "Fonts/DefaultFont.png").ID);
}

/* Точка пересекает прямоугольник? */
bool PointOverBox(glm::vec2 Point, glm::vec2 Pos, glm::vec2 Size) {
	glm::vec2 p = Pos - Size;
	return (
		Point.x >= p.x &&
		Point.x <= p.x + Size.x * 2 &&
		Point.y >= p.y &&
		Point.y <= p.y + Size.y * 2
	);
}

/* Точка пересекает прямоугольник? (Интерфейс) */
bool PointOverUIBox(glm::vec2 Point, bool Resize, glm::vec2 Pos, glm::vec2 Size) {
	return PointOverBox(ScreenPositionToWorld(Point, true, Resize), ScreenPositionToWorld(Pos, true, Resize), Size);
}

/* После обновления интерфейса */
bool FindAnythingUI = false;
int LastHoverUI = -1;
void AfterUpdateUI() {
	if (!FindAnythingUI) {
		MouseOnInterface = false;
		MouseUIObject = -1;
	}

	if (LastHoverUI != MouseUIObject) {
		ButtonHover__(LastHoverUI, false);
		LastHoverUI = MouseUIObject;
	}

	FindAnythingUI = false;
}

/* Обновление интерфейса */
int ui_test = -1;
int ui_test2 = -1;
int t = 0;
void UpdateUI(GameObject& OBJ) {
	if (!FindAnythingUI) {
		bool Over = PointOverUIBox(MousePosition, OBJ.Resize, OBJ.PositionVisual, OBJ.SizeVisual);
		if (Over) {
			MouseOnInterface = true;
			MouseUIObject = OBJ.GetID();
			ButtonHover__(MouseUIObject, true);
			FindAnythingUI = true;
		}
	}

	if (OBJ.Name == "fps") {
		SetGameObjectText(OBJ.GetID(), "FPS: " + StringFPS);
	}
	if (OBJ.Name == "count") {
		SetGameObjectText(OBJ.GetID(), "COUNT: " + std::to_string(Scene.size()));
	}
}

/* Создать интерфейс */
void CreateUI(Scenes Scen) {
	switch (Scen)
	{
		case SCENE_MainMenu: {

			int ui2 = CreateGameObject("ui", RO_UI);
			SetGameObjectSize(ui2, glm::vec2(1.0f, 1.0f));
			SetGameObjectPosition(ui2, glm::vec2(0, -0.5f));
			SetGameObjectResize(ui2, false);
			MakeGameObjectButton(ui2, []() {
				SetScene(SCENE_Game);
			}, []() {});

			ui2 = CreateGameObject("title", RO_UI);
			MakeGameObjectText(ui2, "LITHIUM UNIVERSE");
			SetGameObjectResize(ui2, false);
			SetGameObjectPosition(ui2, glm::vec2(-0.5f, 0.5f));
			SetGameObjectSize(ui2, glm::vec2(2, 2));

			break;
		}
		default: {

			int ui = CreateGameObject("ui", RO_UI);
			SetGameObjectSize(ui, glm::vec2(0.25f, 0.25f));
			SetGameObjectPosition(ui, glm::vec2(-0.9f, -0.9f));

			MakeGameObjectButton(ui, []() {
				//RemoveAllTestObject();
				}, []() {});

			ui_test = ui;

			ui = CreateGameObject("ui", RO_UI);
			SetGameObjectSize(ui, glm::vec2(0.25f, 0.25f));
			SetGameObjectPosition(ui, glm::vec2(-0.75f, -0.9f));

			MakeGameObjectButton(ui, []() {
				UpdateResources();
				}, []() {});

			ui_test2 = ui;
				
			ui = CreateGameObject("fps", RO_UI);
			MakeGameObjectText(ui, "");
			SetGameObjectPosition(ui, glm::vec2(-0.975f, 0.9f));
			SetGameObjectResize(ui, false);

			ui = CreateGameObject("count", RO_UI);
			MakeGameObjectText(ui, "");
			SetGameObjectPosition(ui, glm::vec2(-0.975f, 0.8f));
			SetGameObjectResize(ui, false);

			break;
		}
	}
}