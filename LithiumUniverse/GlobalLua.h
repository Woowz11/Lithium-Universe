#pragma once
#include "sol/sol.hpp";

#include <string>

#include "GameMod.h";

class LUA_Class_KeyPressEvent {
public:
	int Key;
	sol::function Event;
	LUA_Class_KeyPressEvent(int Key_, sol::function Event_) {
		Key = Key_;
		Event = Event_;
	}
};

extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPress;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPressed;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyReleased;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeysPressed;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeysReleased;

void RunScript(const std::string& ScriptPath);
void LoadLua(GameMod Mod);
void UnloadLua();
void InstallLua();