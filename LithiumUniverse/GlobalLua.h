#pragma once
#include "sol/sol.hpp";

#include <string>

#include "GameMod.h";

enum LUA_OBJ_Type {
	L_Unknown = -1,
	L_Nil     = 0,
	L_Int     = 1,
	L_Double  = 2,
	L_Bool    = 3,
	L_String  = 4,
	L_Table   = 5,
	L_Func    = 6
};

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

LUA_OBJ_Type TypeOf(const sol::object& Obj);

std::string LUA_TypeOf(const sol::object& Obj);

void RunScript(const std::string& ScriptPath);
void LoadLua(GameMod Mod);
void UnloadLua();
void InstallLua();