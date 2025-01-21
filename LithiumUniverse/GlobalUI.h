#pragma once
#include <vector>

#include "GameObject.h";
#include "GameData.h";

class Button {
public:
	int ID;
	std::function<void()> WhenLeftClick;
	std::function<void()> WhenRightClick;

	Button(int i, std::function<void()> lc, std::function<void()> rc) {
		ID = i;
		WhenLeftClick = lc;
		WhenRightClick = rc;
	}
};

extern bool GameInFocus;
extern bool MouseOnInterface;
extern int MouseUIObject;
extern std::vector<Button> Buttons;

void MakeGameObjectUI__(int i);
void AfterUpdateUI();
void UpdateUI(GameObject& OBJ);
void CreateUI(Scenes Scen);