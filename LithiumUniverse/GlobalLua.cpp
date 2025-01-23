#include "sol/sol.hpp";

#include <unordered_map>

#include "ExplorerActions.h";
#include "GlobalResources.h";
#include "GlobalLua.h";
#include "GameMod.h";
#include "Console.h";

/* Основа LUA модов */
std::unordered_map<std::string, std::unique_ptr<sol::state>> ModsLUA = {};

/* ==== Ивенты ==== */

/* Ивент на зажатие клавиши */
std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPress = {};
/* Ивент на зажатие клавиши */
std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPressed = {};
/* Ивент на зажатие клавиши */
std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyReleased = {};

/* ==== Код ==== */

/* Отправить простое сообщение в консоль */
void LUA_Print(const std::string& Message) {
	Print("MOD", Message);
}

class LUA_Resources {
public:
	/* Получить скрипт */
	void LoadScript(const std::string& Path) {
		RunScript(Path);
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

class LUA_Controls {
public:
	/* Ивент: клавиша зажата */
	void KeyPress(const int& Key, const std::function<void()>& Func) {
		LUA_Events_KeyPress.push_back(LUA_Class_KeyPressEvent(Key, Func));
	}

	/* Ивент: клавиша нажата */
	void KeyPressed(const int& Key, const std::function<void()>& Func) {
		LUA_Events_KeyPressed.push_back(LUA_Class_KeyPressEvent(Key, Func));
	}

	/* Ивент: клавиша отжата */
	void KeyReleased(const int& Key, const std::function<void()>& Func) {
		LUA_Events_KeyReleased.push_back(LUA_Class_KeyPressEvent(Key, Func));
	}
};

LUA_Controls LUA_Controls_;

/* ==== Инициализация ==== */

void GameLua(sol::state& LUA) {
	LUA["KEY_A"] = sol::as_table(65);
	LUA["KEY_D"] = sol::as_table(68);
	LUA["KEY_S"] = sol::as_table(83);
	LUA["KEY_W"] = sol::as_table(87);

	LUA["Resources"] = &LUA_Resources_;
	LUA.new_usertype<LUA_Resources>(
		"LUA_Resources",
		"LoadScript", &LUA_Resources::LoadScript
	);

	LUA["Game"] = &LUA_Game_;
	LUA.new_usertype<LUA_Game>(
		"LUA_Game",
		"Connect", &LUA_Game::Connect
	);

	LUA["Controls"] = &LUA_Controls_;
	LUA.new_usertype<LUA_Controls>(
		"LUA_Controls",
		"KeyPress", &LUA_Controls::KeyPress,
		"KeyPressed", &LUA_Controls::KeyPressed,
		"KeyReleased", &LUA_Controls::KeyReleased
	);

	LUA.set_function("Print", &LUA_Print);
}

void LoadLua(GameMod Mod) {
	auto LUA = std::make_unique<sol::state>();

	LUA->open_libraries(sol::lib::base);

	GameLua(*LUA);

	ModsLUA[Mod.ID] = std::move(LUA);
}

void UnloadLua() {
	/* Очиста ивентов */
	LUA_Events_KeyPress = {};
	LUA_Events_KeyPressed = {};
	LUA_Events_KeyReleased = {};
	
	ModsLUA.clear();
}

void InstallLua() {
	Print("LUA", "Lua 5.4.2");
}

void RunScript(const std::string& ScriptPath) {
	std::string Script = ReadFile(ComplexToFullPath(ScriptPath));
	std::string ModID = GetBaseFromPath(ScriptPath);

	auto it = ModsLUA.find(ModID);
	if (it != ModsLUA.end()) {
		sol::state& LUA = *(it->second);
		sol::protected_function_result Result = LUA.script(Script, &sol::script_pass_on_error);
		if (!Result.valid()) {
			sol::error ErrorLua = Result;
			Error("LUA", "Error in script $$Y" + ScriptPath + "$$_!");
			Error("LUA", ErrorLua.what());
		}
	}
	else {
		Error("LUA","Failed to call the script because such a mod [" + ModID + "] does not exist! RunScript(\"" + ScriptPath + "\");");
	}
}