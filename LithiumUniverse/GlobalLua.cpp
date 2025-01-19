#include "sol/sol.hpp";

#include "Console.h";

void InstallLua() {
	sol::state LUA;
	LUA.open_libraries(sol::lib::base);
	//LUA.script("print('HELLO FROM LUA!')");

	Print("LUA", "Lua 5.4.2");
}