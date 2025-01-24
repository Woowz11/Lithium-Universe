#include "sol/sol.hpp";

#include <unordered_map>

#include "ExplorerActions.h";
#include "GlobalResources.h";
#include "StringActions.h";
#include "GlobalLua.h";
#include "GameMod.h";
#include "Console.h";

/* Основа LUA модов */
std::unordered_map<std::string, std::unique_ptr<sol::state>> ModsLUA = {};
std::unordered_map<lua_State*, std::string> ModsLUA_LuaState = {};

/* ==== Ивенты ==== */

/* Ивент на зажатие клавиши */
std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPress = {};
/* Ивент на зажатие клавиши */
std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPressed = {};
/* Ивент на зажатие клавиши */
std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyReleased = {};

/* Ивент на зажатие клавиш */
std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeysPressed = {};
/* Ивент на зажатие клавиш */
std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeysReleased = {};

/* ==== Доп. Код ==== */

/* Получить айди мода по lua_State*/
std::string GetBaseFromLuaState(lua_State* L) {
	auto it = ModsLUA_LuaState.find(L);
	if (it != ModsLUA_LuaState.end()) {
		return it->second;
	}
	else {
		Error("LUA","Not found mod base from lua_State*! GetBaseFromLuaState(?);");
		return "Unknown Mod ID";
	}
}

/* Конвертировать объект в число */
double ObjectToDouble(const sol::object& Obj) {
	return Obj.as<double>();
}

/* Конвертировать объект в строку */
std::string ObjectToString(const sol::object& Obj) {
	LUA_OBJ_Type T = TypeOf(Obj);
	switch (T)
	{
	case L_Nil:
		return "nil";
		break;
	case L_Int:
	case L_Double:
		return ToStringNumber(ObjectToDouble(Obj));
		break;
	case L_Bool:
		return ToStringBool(Obj.as<bool>() == true);
		break;
	case L_String:
		return Obj.as<std::string>();
		break;
	case L_Table:
		return "Table";
		break;
	case L_Func:
		return "Function";
		break;
	default:
		return "?";
		break;
	}
}

/* Превратить тип объекта в строку */
std::string LuaObjTypeToString(const LUA_OBJ_Type T) {
	switch (T)
	{
	case L_Nil:
		return "n";
		break;
	case L_Int:
		return "i";
		break;
	case L_Double:
		return "d";
		break;
	case L_Bool:
		return "b";
		break;
	case L_String:
		return "s";
		break;
	case L_Table:
		return "t";
		break;
	case L_Func:
		return "f";
		break;
	default:
		return "?";
		break;
	}
}

/* Получить тип объекта Obj */
LUA_OBJ_Type TypeOf(const sol::object& Obj) {
	switch (Obj.get_type())
	{
	case sol::type::nil: {
		/* Пустая переменная */
		return L_Nil;
	}
	case sol::type::boolean: {
		/* Булево значение */
		return L_Bool;
	}
	case sol::type::number: {
		/* Число */
		double val_i_d = ObjectToDouble(Obj);
		return (std::floor(val_i_d) == val_i_d) ? L_Int : L_Bool;
	}
	case sol::type::string: {
		/* Строка */
		return L_String;
	}
	case sol::type::table: {
		/* Таблица */
		return L_Table;
	}
	case sol::type::function: {
		/* Функция */
		return L_Func;
	}
	default:
		return L_Unknown;
		break;
	}
}

/* Первратить данные о Lua функции в строку */
std::string LuaComplexFunction(const std::string Function, const std::vector<sol::object>& Params) {
	std::string Result = Function + "(";
	for (size_t i = 0; i < Params.size(); i++) {
		Result += ObjectToString(Params[i]);
		if (TypeOf(Params[i]) == L_String) { Result = "\"" + Result + "\""; }
		if (i < Params.size() - 1) {
			Result += ",";
		}
	}
	return Result + ")";
}

void LuaError(std::string ErrorMessage, lua_State* L) {
	std::string ModID = GetBaseFromLuaState(L);
	Error(ModID, ErrorMessage);
	
	lua_Debug LDebug;

	int level = 1;
	int pre_stack_size = lua_gettop(L);
	if (lua_getstack(L, level, &LDebug) != 1) {
		Error(ModID, "Unable to traverse thestack!");
		lua_settop(L, pre_stack_size);
		return;
	}

	if (lua_getinfo(L, "fnluS", &LDebug) == 0) {
		Error(ModID, "Unable to get stack information!");
		lua_settop(L, pre_stack_size);
		return;
	}

	Error(ModID, "[" + std::to_string(level) + "] " + std::string(LDebug.short_src) + " : " + std::to_string(LDebug.currentline) + " - " + (LDebug.name ? LDebug.name : "<unknown>") + " (" + LDebug.what + ")");
	sol::function f(L, -1);
	sol::environment env(sol::env_key, f);
	if (!env.valid()) {
		Error(ModID, "No environment to get!");
	}
	sol::state_view lua(L);
	sol::environment freshenv = lua["freshenv"];
	bool is_same_env = freshenv == env;
	if (!is_same_env) { Error(ModID, "Environments differ!"); }
}

/* Проверить, совпадает ли тип объекта с указанным типом */
bool LuaCheckType(const sol::object& Obj, const LUA_OBJ_Type Type, const std::string Function, const std::vector<sol::object>& Params, lua_State* L) {
	LUA_OBJ_Type T = TypeOf(Obj);
	if (T == Type) {
		return true;
	}
	else {
		LuaError("Variable [" + ObjectToString(Obj) + " (" + LUA_TypeOf(Obj) + ")] must be of type [" + LuaObjTypeToString(Type) + "]! " + LuaComplexFunction(Function, Params), L);
		return false;
	}
}

/* Проверить, совпадает ли тип объекта с указанным типом 1 или типом 2 */
bool LuaCheckType2(const sol::object& Obj, const LUA_OBJ_Type Type1, const LUA_OBJ_Type Type2, const std::string Function, const std::vector<sol::object>& Params, lua_State* L) {
	LUA_OBJ_Type T = TypeOf(Obj);
	if (T == Type1 || T == Type2) {
		return true;
	}
	else {
		LuaError("Variable [" + ObjectToString(Obj) + " (" + LUA_TypeOf(Obj) + ")] must be of type [" + LuaObjTypeToString(Type1) + " || " + LuaObjTypeToString(Type2) + "]! " + LuaComplexFunction(Function, Params), L);
		return false;
	}
}

/* ==== Код ==== */

/* Отправить простое сообщение в консоль */
void LUA_Print(const sol::object& Message, sol::this_state s) {
	lua_State* L = s;
	//if (LuaCheckType(Message, L_String, "Print", {Message}, L)) {
		Print(GetBaseFromLuaState(L), ObjectToString(Message));
	//}
}

/* Получить тип объекта */
std::string LUA_TypeOf(const sol::object& Obj) {
	return LuaObjTypeToString(TypeOf(Obj));
}

/* Конвертировать объект в строку */
std::string LUA_ToString(const sol::object& Obj) {
	return ObjectToString(Obj);
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
	void Connect(const std::string& event_name, const sol::function& func) {
		PrintVeryFast(event_name);
	}
};

LUA_Game LUA_Game_;

class LUA_Controls {
public:
    /* Ивент: клавиша зажата */
    void KeyPress(const int& Key, const sol::function& Func) {
        LUA_Events_KeyPress.push_back(LUA_Class_KeyPressEvent(Key, Func));
    }

    /* Ивент: клавиша нажата */
    void KeyPressed(const int& Key, const sol::function& Func) {
        LUA_Events_KeyPressed.push_back(LUA_Class_KeyPressEvent(Key, Func));
    }

    /* Ивент: клавиша отжата */
    void KeyReleased(const int& Key, const sol::function& Func) {
        LUA_Events_KeyReleased.push_back(LUA_Class_KeyPressEvent(Key, Func));
    }

    /* Ивент: клавиши нажаты */
    void KeysPressed(const sol::function& Func) {
        LUA_Events_KeysPressed.push_back(LUA_Class_KeyPressEvent(-1, Func));
    }

    /* Ивент: клавиши отжаты */
    void KeysReleased(const sol::function& Func) {
        LUA_Events_KeysReleased.push_back(LUA_Class_KeyPressEvent(-1, Func));
    }
};

LUA_Controls LUA_Controls_;

/* ==== Константы ==== */

std::unordered_map<std::string, int> Keys_Constants = { {"SPACE",32},{"APOSTROPHE",39},{"COMMA",44},{"MINUS",45},{"PERIOD",46},{"SLASH",47},{"0",48},{"1",49},{"2",50},{"3",51},{"4",52},{"5",53},{"6",54},{"7",55},{"8",56},{"9",57},{"SEMICOLON",59},{"EQUAL",61},{"A",65},{"B",66},{"C",67},{"D",68},{"E",69},{"F",70},{"G",71},{"H",72},{"I",73},{"J",74},{"K",75},{"L",76},{"M",77},{"N",78},{"O",79},{"P",80},{"Q",81},{"R",82},{"S",83},{"T",84},{"U",85},{"V",86},{"W",87},{"X",88},{"Y",89},{"Z",90},{"LEFT_BRACKET",91},{"BACKSLASH",92},{"RIGHT_BRACKET",93},{"GRAVE_ACCENT",96},{"WORLD_1",161},{"WORLD_2",162},{"ESCAPE",256},{"ENTER",257},{"TAB",258},{"BACKSPACE",259},{"INSERT",260},{"DELETE",261},{"RIGHT",262},{"LEFT",263},{"DOWN",264},{"UP",265},{"PAGE_UP",266},{"PAGE_DOWN",267},{"HOME",268},{"END",269},{"CAPS_LOCK",280},{"SCROLL_LOCK",281},{"NUM_LOCK",282},{"PRINT_SCREEN",283},{"PAUSE",284},{"F1",290},{"F2",291},{"F3",292},{"F4",293},{"F5",294},{"F6",295},{"F7",296},{"F8",297},{"F9",298},{"F10",299},{"F11",300},{"F12",301},{"F13",302},{"F14",303},{"F15",304},{"F16",305},{"F17",306},{"F18",307},{"F19",308},{"F20",309},{"F21",310},{"F22",311},{"F23",312},{"F24",313},{"F25",314},{"KP_0",320},{"KP_1",321},{"KP_2",322},{"KP_3",323},{"KP_4",324},{"KP_5",325},{"KP_6",326},{"KP_7",327},{"KP_8",328},{"KP_9",329},{"KP_DECIMAL",330},{"KP_DIVIDE",331},{"KP_MULTIPLY",332},{"KP_SUBTRACT",333},{"KP_ADD",334},{"KP_ENTER",335},{"KP_EQUAL",336},{"LEFT_SHIFT",340},{"LEFT_CONTROL",341},{"LEFT_ALT",342},{"LEFT_SUPER",343},{"RIGHT_SHIFT",344},{"RIGHT_CONTROL",345},{"RIGHT_ALT",346},{"RIGHT_SUPER",347},{"MENU",348} };

/* ==== Инициализация ==== */

void GameLua(sol::state& LUA) {
	for (auto [KeyName, KeyID] : Keys_Constants) {
		LUA["KEY_"+KeyName] = sol::as_table(KeyID);
	}

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
        "KeyReleased", &LUA_Controls::KeyReleased,
        "KeysPressed", &LUA_Controls::KeysPressed,
        "KeysReleased", &LUA_Controls::KeysReleased
	);

	LUA.set_function("Print", &LUA_Print);
	LUA.set_function("TypeOf", &LUA_TypeOf);
	LUA.set_function("ToString", &LUA_ToString);
}

void LoadLua(GameMod Mod) {
	auto LUA = std::make_unique<sol::state>();

	LUA->open_libraries(sol::lib::base);

	GameLua(*LUA);

	lua_State* L = LUA->lua_state();

	ModsLUA[Mod.ID] = std::move(LUA);
	ModsLUA_LuaState[L] = Mod.ID;
}

void UnloadLua() {
	/* Очиста ивентов */
    LUA_Events_KeyPress     = {};
    LUA_Events_KeyPressed   = {};
    LUA_Events_KeyReleased  = {};
    LUA_Events_KeysPressed  = {};
    LUA_Events_KeysReleased = {};
	
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