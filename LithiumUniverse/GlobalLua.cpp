#include "sol/sol.hpp";

#include "ExplorerActions.h";
#include "GlobalResources.h";
#include "Console.h";

/* Основа LUA */
sol::state LUA;

/* Отправить простое сообщение в консоль */
void LUA_Print(std::string Message) {
	Print("MOD", Message);
}

class LUA_Resources {
public:
	/* Получить скрипт */
	sol::object GetScript(std::string Path) {
		/* МБ сделать так что-бы можно было вызывать функции из других скриптов и получать результат?????? */
		return sol::nil;
	}
};

LUA_Resources LUA_Resources_;

class LUA_Game {
public:
	/* Соеденить функцию с ивентом */
	void Connect(const std::string& event_name, const std::function<void()>& func) {
		PrintVeryFast(event_name);
	}
};

LUA_Game LUA_Game_;

void GameLua() {
	LUA["Resources"] = &LUA_Resources_;
	LUA.new_usertype<LUA_Resources>(
		"LUA_Resources",
		"GetScript", &LUA_Resources::GetScript
	);

	LUA["Game"] = &LUA_Game_;
	LUA.new_usertype<LUA_Game>(
		"LUA_Game",
		"Connect", &LUA_Game::Connect
	);

	LUA.set_function("Print", &LUA_Print);
}

void InstallLua() {
	LUA.open_libraries(sol::lib::base);

	Print("LUA", "Lua 5.4.2");
	
	GameLua();
}

void RunScript(const std::string& ScriptPath) {
	std::string Script = ReadFile(ComplexToFullPath(ScriptPath));
	sol::protected_function_result Result = LUA.script(Script, &sol::script_pass_on_error);
	if (!Result.valid()) {
		sol::error ErrorLua = Result;
		Error("LUA", "Error in script $$Y" + ScriptPath + "$$_!");
		Error("LUA", ErrorLua.what());
	}
}