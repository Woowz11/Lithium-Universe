#include "sol/sol.hpp";

#include <unordered_map>

#include "ExplorerActions.h";
#include "GlobalResources.h";
#include "StringActions.h";
#include "GameInstalls.h";
#include "GlobalLua.h";
#include "GameData.h";
#include "GameMod.h";
#include "Console.h";

/* Основа LUA модов */
std::unordered_map<std::string, std::unique_ptr<sol::state>> ModsLUA = {};
std::unordered_map<lua_State*, std::string> ModsLUA_LuaState = {};

/* ==== Константы ==== */

const double PI = 3.14159265358;

/* ==== Ошибочные переменные ====*/

const double ErrorNumber = -62122.723;
const int ErrorInt       = -6212223;

/* ==== Ивенты ==== */

/* Ивент вызывается каждый кадр */
std::vector<sol::function> LUA_Events_Update = {};

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

/* Конвертировать объект в целое число */
int ObjectToInt(const sol::object& Obj) {
	return (int)std::floor(ObjectToDouble(Obj));
}

/* Конвертировать объект в булеан */
bool ObjectToBool(const sol::object& Obj) {
	return Obj.as<bool>();
}

/* Конвертировать объект в Vector2 */
LUA_Vector2 ObjectToVector2(const sol::object& Obj) {
	return Obj.as<LUA_Vector2>();
}

/* Конвертировать объект в функцию */
sol::function ObjectToFunc(const sol::object& Obj) {
	return Obj.as<sol::function>();
}

/* Конвертировать объект в строку */
std::string ObjectToString(const sol::object& Obj, bool SaveQuotes = false) {
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
		return (SaveQuotes ? "\"" : "") + Obj.as<std::string>() + (SaveQuotes ? "\"" : "");
		break;
	case L_Table:
		return "Table";
		break;
	case L_Func:
		return "Function";
		break;
	case L_Vec2:
		return Obj.as<LUA_Vector2>().ToString();
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
	case L_Vec2:
		return "v2";
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
		return (std::floor(val_i_d) == val_i_d) ? L_Int : L_Double;
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
	case sol::type::userdata: {
		if (Obj.is<LUA_Vector2>()) {
			return L_Vec2;
		}
		return L_Unknown;
		break;
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
		Result += ObjectToString(Params[i], true);
		if (i < Params.size() - 1) {
			Result += ",";
		}
	}
	return Result + ")";
}

/* Первратить данные о Lua операторе в строку */
std::string LuaComplexOperator(const std::string Operator, const std::string ObjA, const sol::object& ObjB) {
	return ObjA + " " + Operator + " " + ObjectToString(ObjB, true);
}

/* Ошибка Lua (короткая) */
void LuaErrorCompact(std::string ErrorMessage) {
	Error("LUA", ErrorMessage);
}

/* Ошибка Lua */
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

/* Сообщить об ошибке, что такой тип не поддерживается в операторе */
void LuaErrorOperator(const std::string& ObjA, const std::string Operator, const sol::object& ObjB) {
	LUA_OBJ_Type T = TypeOf(ObjB);
	LuaErrorCompact("Variable [" + ObjectToString(ObjB, true) + " (" + LUA_TypeOf(ObjB) + ")] not supported! " + LuaComplexOperator(Operator, ObjA, ObjB));
}

/* Проверить, совпадает ли тип объекта с указанным типом */
bool LuaCheckType(const sol::object& Obj, const LUA_OBJ_Type Type, const std::string Function, const std::vector<sol::object>& Params, lua_State* L) {
	LUA_OBJ_Type T = TypeOf(Obj);
	if (T == Type) {
		return true;
	}
	else {
		LuaError("Variable [" + ObjectToString(Obj,true) + " (" + LUA_TypeOf(Obj) + ")] must be of type [" + LuaObjTypeToString(Type) + "]! " + LuaComplexFunction(Function, Params), L);
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
		LuaError("Variable [" + ObjectToString(Obj, true) + " (" + LUA_TypeOf(Obj) + ")] must be of type [" + LuaObjTypeToString(Type1) + " || " + LuaObjTypeToString(Type2) + "]! " + LuaComplexFunction(Function, Params), L);
		return false;
	}
}

/* Проверить, совпадает ли тип объекта с типом дробного или целого числа */
bool LuaCheckNumber(const sol::object& Obj, const std::string Function, const std::vector<sol::object>& Params, lua_State* L) {
	return LuaCheckType2(Obj, L_Int, L_Double, Function, Params, L);
}

/* ==== Код ==== */

/* Отправить простое сообщение в консоль */
void LUA_Print(const sol::object& Message, sol::this_state s) {
	lua_State* L = s;
	Print(GetBaseFromLuaState(L), ObjectToString(Message));
}

/* Отправить простое сообщение в консоль (очень быстро, без логов и т.д) */
void LUA_PrintFast(const sol::object& Message, sol::this_state s) {
	lua_State* L = s;
	PrintVeryFast(ObjectToString(Message));
}

/* Получить тип объекта */
std::string LUA_TypeOf(const sol::object& Obj) {
	return LuaObjTypeToString(TypeOf(Obj));
}

/* Конвертировать объект в строку */
std::string LUA_ToString(const sol::object& Obj) {
	return ObjectToString(Obj);
}

/* Типо условия x ? y : z */
sol::object LUA_IfThen(const sol::object& Boolean, const sol::object& ObjA, const sol::object& ObjB, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckType(Boolean, L_Bool, "IfThen", {Boolean, ObjA, ObjB}, L)) {
		return ObjectToBool(Boolean) ? ObjA : ObjB;
	}
	return ObjB;
}

/* Сделать число положительным */
double LUA_Abs(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Abs", { Value }, L)) {
		return abs(ObjectToDouble(Value));
	}
	return ErrorNumber;
}

/* Возвести число в синус */
double LUA_Sin(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Sin", { Value }, L)) {
		return sin(ObjectToDouble(Value));
	}
	return ErrorNumber;
}

/* Возвести корень */
double LUA_Sqrt(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Sqrt", { Value }, L)) {
		double D = ObjectToDouble(Value);
		return sqrt(abs(D)) * (D<0?-1:1);
	}
	return ErrorNumber;
}

/* Получить DeltaTime */
double LUA_DeltaTime() {
	return DeltaTime;
}

class LUA_Resources {
public:
	/* Получить скрипт */
	void LoadScript(const std::string& Path) {
		RunScript(Path);
	}
};

LUA_Resources LUA_Resources_Instance;

class LUA_Game {
public:
	/* Ивент: каждый кадр выполнять */
	void Update(const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Func, L_Func, "Game:Update", { Func }, L)) {
			LUA_Events_Update.push_back(ObjectToFunc(Func));
		}
	}
};

LUA_Game LUA_Game_Instance;

class LUA_Controls {
public:
    /* Ивент: клавиша зажата */
    void KeyPress(const sol::object& Key, const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Key, L_Int, "Controls:KeyPress", { Key,Func }, L)) {
			if (LuaCheckType(Func, L_Func, "Controls:KeyPress", { Key,Func }, L)) {
				LUA_Events_KeyPress.push_back(LUA_Class_KeyPressEvent(ObjectToInt(Key), ObjectToFunc(Func)));
			}
		}
    }

    /* Ивент: клавиша нажата */
    void KeyPressed_(const sol::object& Key, const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Key, L_Int, "Controls:KeyPressed", { Key,Func }, L)) {
			if (LuaCheckType(Func, L_Func, "Controls:KeyPressed", { Key,Func }, L)) {
				LUA_Events_KeyPressed.push_back(LUA_Class_KeyPressEvent(ObjectToInt(Key), ObjectToFunc(Func)));
			}
		}
    }

    /* Ивент: клавиша отжата */
    void KeyReleased(const sol::object& Key, const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Key, L_Int, "Controls:KeyReleased", { Key,Func }, L)) {
			if (LuaCheckType(Func, L_Func, "Controls:KeyReleased", { Key,Func }, L)) {
				LUA_Events_KeyReleased.push_back(LUA_Class_KeyPressEvent(ObjectToInt(Key), ObjectToFunc(Func)));
			}
		}
    }

    /* Ивент: клавиши нажаты */
    void KeysPressed(const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Func, L_Func, "Controls:KeysPressed", { Func }, L)) {
			LUA_Events_KeysPressed.push_back(LUA_Class_KeyPressEvent(-1, Func));
		}
    }

    /* Ивент: клавиши отжаты */
    void KeysReleased(const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Func, L_Func, "Controls:KeysReleased", { Func }, L)) {
			LUA_Events_KeysReleased.push_back(LUA_Class_KeyPressEvent(-1, Func));
		}
    }

	/* Клавиша нажата в данный момент? */
	bool KeyIsPressed(const sol::object& Key, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Key, L_Int, "Controls:KeyIsPressed", { Key }, L)) {
			return KeyPressed(ObjectToInt(Key));
		}
		return false;
	}
};

LUA_Controls LUA_Controls_Instance;

class LUA_Camera {
public:
	/* Подвинуть камеру */
	void Move(const sol::object& Dir, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Dir, L_Vec2, "Camera:Move", { Dir }, L)) {
			LUA_Vector2 V2 = ObjectToVector2(Dir);
			Camera->MoveCamera(V2.x, V2.y, DeltaTime);
		}
	}

	/* Изменить зум камере */
	void MoveZoom(const sol::object& Zoom, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckNumber(Zoom, "Camera:MoveZoom", { Zoom }, L)) {
			Camera->MoveCameraZoom(ObjectToDouble(Zoom), DeltaTime);
		}
	}

	/* Повернуть камеру */
	void Rotate(const sol::object& Angle, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckNumber(Angle, "Camera:Rotate", { Angle }, L)) {
			Camera->MoveCameraRotation(ObjectToDouble(Angle), DeltaTime);
		}
	}

	/* Установить позицию камере */
	void SetPosition(const sol::object& Pos, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Pos, L_Vec2, "Camera:SetPosition", { Pos }, L)) {
			LUA_Vector2 V2 = ObjectToVector2(Pos);
			Camera->SetCameraPosition(V2.x, V2.y);
		}
	}

	/* Установить зум камере */
	void SetZoom(const sol::object& Zoom, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckNumber(Zoom, "Camera:SetZoom", { Zoom }, L)) {
			Camera->SetCameraZoom(ObjectToDouble(Zoom));
		}
	}

	/* Установить поворот камере */
	void SetOrientation(const sol::object& Angle, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckNumber(Angle, "Camera:SetOrientation", { Angle }, L)) {
			Camera->SetCameraRotation(ObjectToDouble(Angle));
		}
	}

	/* Вернуть камеру в начальное положение */
	void Reset() {
		Camera->ResetCamera();
	}
};

LUA_Camera LUA_Camera_Instance;

/* ==== Константы ==== */

std::unordered_map<std::string, int> Keys_Constants = { {"SPACE",32},{"APOSTROPHE",39},{"COMMA",44},{"MINUS",45},{"PERIOD",46},{"SLASH",47},{"0",48},{"1",49},{"2",50},{"3",51},{"4",52},{"5",53},{"6",54},{"7",55},{"8",56},{"9",57},{"SEMICOLON",59},{"EQUAL",61},{"A",65},{"B",66},{"C",67},{"D",68},{"E",69},{"F",70},{"G",71},{"H",72},{"I",73},{"J",74},{"K",75},{"L",76},{"M",77},{"N",78},{"O",79},{"P",80},{"Q",81},{"R",82},{"S",83},{"T",84},{"U",85},{"V",86},{"W",87},{"X",88},{"Y",89},{"Z",90},{"LEFT_BRACKET",91},{"BACKSLASH",92},{"RIGHT_BRACKET",93},{"GRAVE_ACCENT",96},{"WORLD_1",161},{"WORLD_2",162},{"ESCAPE",256},{"ENTER",257},{"TAB",258},{"BACKSPACE",259},{"INSERT",260},{"DELETE",261},{"RIGHT",262},{"LEFT",263},{"DOWN",264},{"UP",265},{"PAGE_UP",266},{"PAGE_DOWN",267},{"HOME",268},{"END",269},{"CAPS_LOCK",280},{"SCROLL_LOCK",281},{"NUM_LOCK",282},{"PRINT_SCREEN",283},{"PAUSE",284},{"F1",290},{"F2",291},{"F3",292},{"F4",293},{"F5",294},{"F6",295},{"F7",296},{"F8",297},{"F9",298},{"F10",299},{"F11",300},{"F12",301},{"F13",302},{"F14",303},{"F15",304},{"F16",305},{"F17",306},{"F18",307},{"F19",308},{"F20",309},{"F21",310},{"F22",311},{"F23",312},{"F24",313},{"F25",314},{"KP_0",320},{"KP_1",321},{"KP_2",322},{"KP_3",323},{"KP_4",324},{"KP_5",325},{"KP_6",326},{"KP_7",327},{"KP_8",328},{"KP_9",329},{"KP_DECIMAL",330},{"KP_DIVIDE",331},{"KP_MULTIPLY",332},{"KP_SUBTRACT",333},{"KP_ADD",334},{"KP_ENTER",335},{"KP_EQUAL",336},{"LEFT_SHIFT",340},{"LEFT_CONTROL",341},{"LEFT_ALT",342},{"LEFT_SUPER",343},{"RIGHT_SHIFT",344},{"RIGHT_CONTROL",345},{"RIGHT_ALT",346},{"RIGHT_SUPER",347},{"MENU",348} };

/* ==== Инициализация ==== */

void GameLua(sol::state& LUA) {
	/* Константы */
	for (auto [KeyName, KeyID] : Keys_Constants) {
		LUA["KEY_"+KeyName] = sol::as_table(KeyID);
	}
	LUA["ErrorDouble"] = sol::as_table(ErrorNumber);
	LUA["ErrorInt"] = sol::as_table(ErrorInt);
	LUA["PI"] = sol::as_table(PI);

	/* Классы */
	LUA.new_usertype<LUA_Vector2>("Vector2",
		"X", &LUA_Vector2::x,
		"Y", &LUA_Vector2::y,
		"Length", &LUA_Vector2::Length,
		"ToString", &LUA_Vector2::ToString,
		"Abs", &LUA_Vector2::Abs,
		sol::meta_function::addition, &LUA_Vector2::operator+,
		sol::meta_function::subtraction, &LUA_Vector2::operator-,
		sol::meta_function::multiplication, &LUA_Vector2::operator*
	);
	LUA.set_function("Vector2", [](sol::object x, sol::object y, sol::this_state s) {
		lua_State* L = s;
		if (y == sol::nil) {
			if (x == sol::nil) {
				return LUA_Vector2(0,0);
			}
			else {
				if (LuaCheckNumber(x, "Vector2", { x,x }, L)) {
					double d = ObjectToDouble(x);
					return LUA_Vector2(d, d);
				}
			}
		}
		else {
			if (LuaCheckNumber(x, "Vector2", { x,y }, L)) {
				if (LuaCheckNumber(y, "Vector2", { x,y }, L)) {
					return LUA_Vector2(ObjectToDouble(x), ObjectToDouble(y));
				}
			}
		}
		return LUA_Vector2(0, 0);
	});

	/* Функции*/
	LUA["Resources"] = &LUA_Resources_Instance;
	LUA.new_usertype<LUA_Resources>(
		"LUA_Resources",
		"LoadScript", &LUA_Resources::LoadScript
	);

	LUA["Game"] = &LUA_Game_Instance;
	LUA.new_usertype<LUA_Game>(
		"LUA_Game",
		"Update", &LUA_Game::Update
	);

	LUA["Controls"] = &LUA_Controls_Instance;
	LUA.new_usertype<LUA_Controls>(
		"LUA_Controls",
        "KeyPress", &LUA_Controls::KeyPress,
        "KeyPressed", &LUA_Controls::KeyPressed_,
        "KeyReleased", &LUA_Controls::KeyReleased,
        "KeysPressed", &LUA_Controls::KeysPressed,
        "KeysReleased", &LUA_Controls::KeysReleased,
		"KeyIsPressed", &LUA_Controls::KeyIsPressed
	);

	LUA["Camera"] = &LUA_Camera_Instance;
	LUA.new_usertype<LUA_Camera>(
		"LUA_Camera",
		"Move", &LUA_Camera::Move,
		"MoveZoom", &LUA_Camera::MoveZoom,
		"Rotate", &LUA_Camera::Rotate,
		"SetPosition", &LUA_Camera::SetPosition,
		"SetZoom", &LUA_Camera::SetZoom,
		"SetOrientation", &LUA_Camera::SetOrientation,
		"Reset", &LUA_Camera::Reset
	);

	/* Локальные функции */
	LUA.set_function("Abs", &LUA_Abs);
	LUA.set_function("Sin", &LUA_Sin);
	LUA.set_function("Sqrt", &LUA_Sqrt);
	LUA.set_function("Print", &LUA_Print);
	LUA.set_function("IfThen", &LUA_IfThen);
	LUA.set_function("TypeOf", &LUA_TypeOf);
	LUA.set_function("ToString", &LUA_ToString);
	LUA.set_function("DeltaTime", &LUA_DeltaTime);
	LUA.set_function("PrintFast", &LUA_PrintFast);
}

void LoadLua(GameMod Mod) {
	auto LUA = std::make_unique<sol::state>();

	GameLua(*LUA);

	lua_State* L = LUA->lua_state();

	ModsLUA[Mod.ID] = std::move(LUA);
	ModsLUA_LuaState[L] = Mod.ID;
}

void UnloadLua() {
	/* Очистка ивентов */
	LUA_Events_Update       = {};
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