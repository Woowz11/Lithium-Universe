#pragma once
#include <string>

#include "GameMod.h";

class LUA_Class_KeyPressEvent {
public:
	int Key;
	std::function<void()> Event;
	LUA_Class_KeyPressEvent(int Key_, std::function<void()> Event_) {
		Key = Key_;
		Event = Event_;
	}
};

extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPress;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPressed;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyReleased;

void RunScript(const std::string& ScriptPath);
void LoadLua(GameMod Mod);
void UnloadLua();
void InstallLua();