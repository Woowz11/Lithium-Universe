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
	if (!CheckOutSceneIndex(i)) {
		GameObject& OBJ = GetGameObject(i, "ButtonHover__(" + std::to_string(i) + "," + ToStringBool(Hover) + ");");

		for (const sol::function& F : OBJ.MouseHoverEvent) {
			F(Hover, i);
		}

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
	Button B = Button(i,Bi,WhenLeftClick,WhenRightClick);
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
bool PointOverBox(const glm::vec2 Point, const glm::vec2 Pos, const glm::vec2 Size, const glm::vec2 Center) {
	glm::vec2 p = (Pos - (Center * Size)) - Size;
	return (
		Point.x >= p.x &&
		Point.x <= p.x + Size.x * 2 &&
		Point.y >= p.y &&
		Point.y <= p.y + Size.y * 2
	);
}

/* Точка пересекает прямоугольник? (Интерфейс) */
bool PointOverUIBox(const glm::vec2 Point, const bool Resize, const glm::vec2 Pos, const glm::vec2 Size, const glm::vec2 Center) {
	return PointOverBox(ScreenPositionToWorld(Point, true, Resize), ScreenPositionToWorld(Pos, true, Resize), Size, Center);
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

int DUI_FPS = -1;
int DUI_FPSR = -1;
int DUI_COUNT = -1;

int t = 0;
void UpdateUI(GameObject& OBJ) {
	if (!FindAnythingUI) {
		bool Over = PointOverUIBox(MousePosition, OBJ.Resize, OBJ.PositionVisual, OBJ.SizeVisual, OBJ.Center);
		if (Over) {
			MouseOnInterface = true;
			MouseUIObject = OBJ.GetID();
			ButtonHover__(MouseUIObject, true);
			FindAnythingUI = true;
		}
	}

	int ID = OBJ.GetID();

	if (ID = DUI_FPS) {
		SetGameObjectText(ID, "FPS: " + StringFPS);
	}
	if (ID = DUI_FPSR) {
		t++;
		if (t > 30) {
			glm::vec4 C = glm::vec4(1, FPS / 300, FPS / 1000, 1);
			SetGameObjectText(ID, "FPS-R: " + std::to_string(FPS));
			SetGameObjectColor(ID, C);
			t = 0;
		}
	}
	if (ID = DUI_COUNT) {
		SetGameObjectText(ID, "COUNT: " + std::to_string(TotalSceneGameObjects - TotalDeletedGameObjects));
	}
}

int CreateDebugUILine(int x) {
	int ui = CreateGameObject("[LU] DebugUI", RO_UI);
	MakeGameObjectText(ui, "");
	SetGameObjectPosition(ui, glm::vec2(-0.975f, 0.9f - ((float)x*0.1f)));
	SetGameObjectCenter(ui, glm::vec2(-1, 0));
	SetGameObjectResize(ui, false);
	return ui;
}

/* Создать отладочный интерфейс */
void CreateDebugUI() {
	DUI_FPS   = CreateDebugUILine(0);
	DUI_FPSR  = CreateDebugUILine(1);
	DUI_COUNT = CreateDebugUILine(2);
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
			MakeGameObjectText(ui2, "Lithium Universe");
			SetGameObjectResize(ui2, false);
			SetGameObjectCenter(ui2, glm::vec2(0, 0));
			SetGameObjectPosition(ui2, glm::vec2(0, 0.5f));
			SetGameObjectSize(ui2, glm::vec2(2, 2));

			ui2 = CreateGameObject("title2", RO_UI);
			MakeGameObjectText(ui2, Version);
			SetGameObjectResize(ui2, false);
			SetGameObjectPosition(ui2, glm::vec2(0, 0.4f));

			ui2 = CreateGameObject("title3", RO_UI);
			MakeGameObjectText(ui2, "by Woowz11");
			SetGameObjectResize(ui2, false);
			SetGameObjectPosition(ui2, glm::vec2(0, 0.3f));
			
			ui2 = CreateGameObject("title4", RO_UI);
			MakeGameObjectText(ui2, "https://woowz11.github.io/woowzsite/lithiumuniverse");
			SetGameObjectResize(ui2, false);
			SetGameObjectPosition(ui2, glm::vec2(0, 0.2f));
			SetGameObjectSize(ui2, glm::vec2(0.75, 0.75));

			CreateDebugUI();

			break;
		}
		default: {

			CreateDebugUI();

			break;
		}
	}
}