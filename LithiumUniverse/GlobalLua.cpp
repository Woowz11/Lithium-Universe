#include "sol/sol.hpp";

void InstallLua() {
	sol::state LUA;
	LUA.open_libraries(sol::lib::base);
	LUA.script("print('HELLO FROM LUA!')");
}