#include "sol/sol.hpp";

#include <unordered_map>
#include <windows.h>
#include <iostream>
#include <utility>
#include <string>
#include <regex>

#include "GameObjectActions.h";
#include "ExplorerActions.h";
#include "GlobalResources.h";
#include "StringActions.h";
#include "GlobalPhysic.h";
#include "GlobalRender.h";
#include "GameInstalls.h";
#include "GlobalLua.h";
#include "GlobalUI.h";
#include "GameData.h";
#include "GameMod.h";
#include "Console.h";

/* Основа LUA модов */
std::unordered_map<std::string, std::unique_ptr<sol::state>> ModsLUA = {};
std::unordered_map<lua_State*, std::string> ModsLUA_LuaState = {};

/* ==== Константы ==== */

const double PI = 3.14159265358;

/* ==== Ошибочные переменные ====*/

const double ErrorNumber       = -62122.723;
const int ErrorInt             = -6212223;
const LUA_Vector2 ErrorVector2 = LUA_Vector2(ErrorNumber, ErrorNumber);
const std::string ErrorString  = "Error_GwevWET23g3#G_#1d";

const std::string ErrorTexture = "Base:Textures/Error/NotFound.png";
const std::string ErrorShader = "Base:Shaders/Error.lu_shader";

/* ==== Ивенты ==== */

/* Ивент вызывается каждый кадр */
std::vector<sol::function> LUA_Events_Update = {};
/* Ивент вызывается каждый кадр для каждого модового объекта */
std::vector<sol::function> LUA_Events_UpdateEveryGameObject = {};
/* Ивент вызывается когда игровой объект удалён */
std::vector<sol::function> LUA_Events_GameObjectDeleted = {};

/* Ивент вызывается при загрузке игровых объектов */
std::vector<sol::function> LUA_Events_GameObjectLoading = {};
/* Ивент вызывается при загрузке интерфейса */
std::vector<sol::function> LUA_Events_UILoading = {};

/* Ивент вызывается когда двигается колёсико мыши */
std::vector<sol::function> LUA_Events_MouseScroll = {};

/* Ивент вызывается когда нажимается на клавишу на мышке */
std::vector<sol::function> LUA_Events_MousePressed = {};
/* Ивент вызывается когда отжимается клавиша на мышке */
std::vector<sol::function> LUA_Events_MouseReleased = {};

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
int ObjectToInt(const sol::object& Obj, bool ThatMaybeDouble = false) {
	if (ThatMaybeDouble) {
		return (int)std::floor(ObjectToDouble(Obj));
	}
	else {
		return (int)ObjectToDouble(Obj);
	}
}

/* Конвертировать объект в булеан */
bool ObjectToBool(const sol::object& Obj) {
	return Obj.as<bool>();
}

/* Конвертировать объект в Vector2 */
LUA_Vector2 ObjectToVector2(const sol::object& Obj) {
	return Obj.as<LUA_Vector2>();
}

/* Конвертировать объект в Color */
LUA_Color ObjectToColor(const sol::object& Obj) {
	return Obj.as<LUA_Color>();
}

/* Конвертировать объект в функцию */
sol::function ObjectToFunc(const sol::object& Obj) {
	return Obj.as<sol::function>();
}

/* Конвертировать объект в таблицу */
sol::table ObjectToTable(const sol::object& Obj) {
	return Obj.as<sol::table>();
}

/* Конвертировать объект в строку */
std::string ObjectToString(const sol::object& Obj, bool SaveQuotes = false, int TableHierarchy = 0) {
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
		if (TableHierarchy>0) {
			return TableToString(Obj, TableHierarchy);
		}
		else {
			int TableSize = 0;
			for (const auto& pair : Obj.as<sol::table>()) {
				TableSize++;
			}
			return "Table{" + std::to_string(TableSize) + "}";
		}
		break;
	case L_Func:
		return "Function";
		break;
	case L_Vec2:
		return Obj.as<LUA_Vector2>().ToString();
		break;
	case L_Color:
		return Obj.as<LUA_Color>().ToString();
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
	case L_Color:
		return "c";
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
		if (Obj.is<LUA_Color>()) {
			return L_Color;
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
void LuaErrorCompact(const std::string ErrorMessage) {
	Error("LUA", ErrorMessage);
}

/* Ошибка Lua (кастомная) */
void LuaErrorCustom(const std::string ModID, const std::string ErrorCode, const int ErrorLine, std::string What) {
	Error(ModID, (ErrorCode.empty() ? "" : ErrorCode + " $$R:$$_ ") + std::to_string(ErrorLine) + " $$R: " + What);
}

/* Ошибка Lua */
void LuaError(const std::string ErrorMessage, lua_State* L) {
	std::string ModID = GetBaseFromLuaState(L);
	
	lua_Debug LDebug;

	int level = 1;
	int pre_stack_size = lua_gettop(L);
	if (lua_getstack(L, level, &LDebug) != 1) {
		Error(ModID, ErrorMessage);
		Error(ModID, "$$RUnable to traverse thestack!");
		lua_settop(L, pre_stack_size);
		return;
	}

	if (lua_getinfo(L, "fnluS", &LDebug) == 0) {
		Error(ModID, ErrorMessage);
		Error(ModID, "$$RUnable to get stack information!");
		lua_settop(L, pre_stack_size);
		return;
	}

	std::string Code = LDebug.source;
	int ErrorLine = LDebug.currentline;

	std::string ErrorCode = GetLineFromString(Code, ErrorLine);

	LuaErrorCustom(ModID, ErrorCode, ErrorLine, ErrorMessage);
}

/* Сообщить об ошибке, что такой тип не поддерживается в операторе */
void LuaErrorOperator(const std::string& ObjA, const std::string Operator, const sol::object& ObjB) {
	LUA_OBJ_Type T = TypeOf(ObjB);
	LuaErrorCompact("$$RVariable [$$_" + ObjectToString(ObjB, true) + " $$R($$_" + LUA_TypeOf(ObjB) + "$$R)] not supported! $$_" + LuaComplexOperator(Operator, ObjA, ObjB));
}

/* Проверить, совпадает ли тип объекта с указанным типом */
bool LuaCheckType(const sol::object& Obj, const LUA_OBJ_Type Type, const std::string Function, const std::vector<sol::object>& Params, lua_State* L) {
	LUA_OBJ_Type T = TypeOf(Obj);
	if (T == Type) {
		return true;
	}
	else {
		LuaError("$$RVariable [$$_" + ObjectToString(Obj,true) + " $$R($$_" + LUA_TypeOf(Obj) + "$$R)] must be of type [$$_" + LuaObjTypeToString(Type) + "$$R]! $$_" + LuaComplexFunction(Function, Params), L);
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
		LuaError("$$RVariable [$$_" + ObjectToString(Obj, true) + " $$R($$_" + LUA_TypeOf(Obj) + "$$R)] must be of type [$$_" + LuaObjTypeToString(Type1) + " || " + LuaObjTypeToString(Type2) + "$$R]! $$_" + LuaComplexFunction(Function, Params), L);
		return false;
	}
}

/* Проверить, подходит ли игровой объект? */
bool LuaCheckGameObject(const sol::object& Obj, const std::string Function, const std::vector<sol::object>& Params, lua_State* L) {
	LUA_OBJ_Type T = TypeOf(Obj);
	if (T == L_Int) {
		int ObjID = ObjectToInt(Obj);
		if (CheckOutSceneIndex(ObjID)) {
			LuaError("$$RGameObject [$$_" + std::to_string(ObjID) + "$$R] is outside of ID's Scene!$$_ " + LuaComplexFunction(Function, Params), L);
			return false;
		}
		else {
			GameObject& OBJ = GetGameObject(ObjID, "LuaCheckGameObject(?,\"" + Function + "\",?,?);");
			if (!OBJ.Deleted) {
				if (OBJ.CreatedFromMods) {
					return true;
				}
				else {
					LuaError("$$RGameObject[$$_" + std::to_string(ObjID) + "$$R] can't be obtained because it's not a moddable GameObject!$$_ " + LuaComplexFunction(Function, Params), L);
					return false;
				}
			}
			else {
				LuaError("$$RGameObject[$$_" + std::to_string(ObjID) + "$$R] can't be obtained because the GameObject has been deleted!$$_ " + LuaComplexFunction(Function, Params), L);
				return false;
			}
		}
	}
	else {
		LuaError("$$RVariable [$$_" + ObjectToString(Obj, true) + " $$R($$_" + LUA_TypeOf(Obj) + "$$R)] must be of type [$$_" + LuaObjTypeToString(L_Int) + "$$R]! $$_" + LuaComplexFunction(Function, Params), L);
		return false;
	}
}

/* Проверить, совпадает ли тип объекта с типом дробного или целого числа */
bool LuaCheckNumber(const sol::object& Obj, const std::string Function, const std::vector<sol::object>& Params, lua_State* L) {
	return LuaCheckType2(Obj, L_Int, L_Double, Function, Params, L);
}

/* Превратить таблицу в строку */
std::string TableToString(const sol::table& Table, int Hierarchy) {
	if (!Table.valid() || Table.empty()) {
		return "{}";
	}
	else {
		std::string Result = "{\n";
		for (const auto& pair : Table) {
			sol::object Key = pair.first;
			sol::object Value = pair.second;
			Result += RepeatString("   ", Hierarchy + 1) + "[" + ObjectToString(Key, true, Hierarchy + 1) + "] = " + ObjectToString(Value, true, Hierarchy + 1) + ",\n";
		}
		return Result + RepeatString("   ", Hierarchy) + "}";
	}
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
	return ObjectToString(Obj,true);
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

/* Возвести число в косинус */
double LUA_Cos(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Cos", { Value }, L)) {
		return cos(ObjectToDouble(Value));
	}
	return ErrorNumber;
}

/* Округлить число */
double LUA_Round(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Round", { Value }, L)) {
		return floor(ObjectToDouble(Value) + 0.5);
	}
	return ErrorNumber;
}

/* Округлить число (при 0.5 равно 0) */
double LUA_RoundLower(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "RoundLower", { Value }, L)) {
		return floor(ObjectToDouble(Value) + 0.49999);
	}
	return ErrorNumber;
}

/* Убрать дробную часть числа */
double LUA_Floor(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Floor", { Value }, L)) {
		return floor(ObjectToDouble(Value));
	}
	return ErrorNumber;
}

/* Если число дробное, убрать дробную часть, и +1 к числу */
double LUA_Ceil(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Ceil", { Value }, L)) {
		return ceil(ObjectToDouble(Value));
	}
	return ErrorNumber;
}

/* Убрать дробную часть числа */
double LUA_Trunc(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Trunc", { Value }, L)) {
		double D = ObjectToDouble(Value);
		double N = (D < 0 ? -1 : 1);
		return floor(abs(D)) * N;
	}
	return ErrorNumber;
}

/* Возвести в квадратный корень */
double LUA_Sqrt(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Sqrt", { Value }, L)) {
		double D = ObjectToDouble(Value);
		return sqrt(abs(D)) * (D < 0 ? -1 : 1);
	}
	return ErrorNumber;
}

/* Возвести в кубический корень */
double LUA_Cbrt(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Cbrt", { Value }, L)) {
		return cbrt(ObjectToDouble(Value));
	}
	return ErrorNumber;
}

/* Получить знак числа */
double LUA_Sgn(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Sgn", { Value }, L)) {
		double D = ObjectToDouble(Value);
		return (D==0?0:(D>0?1:-1));
	}
	return ErrorNumber;
}

/* Получить максимальное число */
double LUA_Max(const sol::object& ValueA, const sol::object& ValueB, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(ValueA, "Max", { ValueA, ValueB }, L)) {
		if (LuaCheckNumber(ValueB, "Max", { ValueA, ValueB }, L)) {
			double A = ObjectToDouble(ValueA);
			double B = ObjectToDouble(ValueB);
			return A > B ? A : B;
		}
	}
	return ErrorNumber;
}

/* Получить минимальное число */
double LUA_Min(const sol::object& ValueA, const sol::object& ValueB, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(ValueA, "Min", { ValueA, ValueB }, L)) {
		if (LuaCheckNumber(ValueB, "Min", { ValueA, ValueB }, L)) {
			double A = ObjectToDouble(ValueA);
			double B = ObjectToDouble(ValueB);
			return A < B ? A : B;
		}
	}
	return ErrorNumber;
}

/* Градус -> радиан */
double LUA_Rad(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Rad", { Value }, L)) {
		return ObjectToDouble(Value) * (PI/180);
	}
	return ErrorNumber;
}

/* Радиан -> градус */
double LUA_Deg(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckNumber(Value, "Deg", { Value }, L)) {
		return ObjectToDouble(Value) * (180/PI);
	}
	return ErrorNumber;
}

/* Получить DeltaTime */
double LUA_DeltaTime() {
	return DeltaTime;
}

/* Получить мировую позицию мыши */
LUA_Vector2 LUA_MouseWorldPosition() {
	return LUA_Vector2(MouseWorldPosition.x, MouseWorldPosition.y);
}

/* Получить позицию мыши (размер окна не виляет на него) */
LUA_Vector2 LUA_MouseLocalPosition() {
	return LUA_Vector2(MousePositionNonResize.x, MousePositionNonResize.y);
}

/* Получить позицию мыши */
LUA_Vector2 LUA_MousePosition() {
	return LUA_Vector2(MousePosition.x, MousePosition.y);
}

/* Сделать экранную позицию в мировую */
LUA_Vector2 LUA_ScreenToWorldPosition(const sol::object& Value, sol::this_state s) {
	lua_State* L = s;
	if (LuaCheckType(Value, L_Vec2, "ScreenToWorldPosition", { Value }, L)) {
		LUA_Vector2 V2 = ObjectToVector2(Value);
		glm::vec2 Result = ScreenPositionToWorld(glm::vec2(V2.x, V2.y), false, true);
		return LUA_Vector2(Result.x, Result.y);
	}
	return ErrorVector2;
}

/* Возвращает быстро случайное число от 0 до 1 */
double LUA_RandomFast() {
	return (static_cast<double>(rand()) / RAND_MAX);
}

class LUA_OS {
public:
	/* Получить текущий язык системы */
	std::string GetSystemLanguage(sol::this_state s) {
		lua_State* L = s;

		LANGID LangID = GetUserDefaultUILanguage();
		char LangName[LOCALE_NAME_MAX_LENGTH];
		if (GetLocaleInfoA(LangID, LOCALE_SENGLISHLANGUAGENAME, LangName, LOCALE_NAME_MAX_LENGTH) > 0) {
			return LangName;
		}
		else {
			LuaError("Failed to get system language! " + LuaComplexFunction("OS:GetSystemLanguage", {}), L);
			return ErrorString;
		}
	}
};

LUA_OS LUA_OS_Instance;

class LUA_Game {
public:
	/* Ивент: выполняется каждый кадр */
	void Update(const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Func, L_Func, "Game:Update", { Func }, L)) {
			LUA_Events_Update.push_back(ObjectToFunc(Func));
		}
	}

	/* Ивент: выполняется каждый кадр для каждого объекта */
	void UpdateEveryGameObject(const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Func, L_Func, "Game:UpdateEveryGameObject", { Func }, L)) {
			LUA_Events_UpdateEveryGameObject.push_back(ObjectToFunc(Func));
		}
	}

	/* Ивент: выполняется каждый раз, когда игровой объект удалён */
	void GameObjectDeleted(const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Func, L_Func, "Game:GameObjectDeleted", { Func }, L)) {
			LUA_Events_GameObjectDeleted.push_back(ObjectToFunc(Func));
		}
	}

	/* Ивент: вызывается при загрузке игровых объектов */
	void GameObjectLoading(const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Func, L_Func, "Game:GameObjectLoading", { Func }, L)) {
			LUA_Events_GameObjectLoading.push_back(ObjectToFunc(Func));
		}
	}

	/* Ивент: вызывается при загрузке интерфейса */
	void UILoading(const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Func, L_Func, "Game:UILoading", { Func }, L)) {
			LUA_Events_UILoading.push_back(ObjectToFunc(Func));
		}
	}

	/* Установить скорость времени */
	void SetSimulationSpeed_(const sol::object& NewST, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckNumber(NewST, "Game:SetSimulationSpeed", { NewST }, L)) {
			SetSimulationSpeed(ObjectToDouble(NewST));
		}
	}

	/* Получить полную версию игры */
	std::string GetFullVersion() {
		return Version;
	}
};

LUA_Game LUA_Game_Instance;

class LUA_Table {
public:
	/* Удаление ключа из таблицы */
	void RemoveKey(sol::object Table, const sol::object& Key, const sol::object& RemoveFromKeys, sol::this_state s) {
		lua_State* L = s;

		if (Key == sol::nil) {
			LuaError("The key cannot be nil! " + LuaComplexFunction("Table:RemoveKey", { Table, Key, RemoveFromKeys }), L);
			return;
		}

		bool RFK = false;
		if (RemoveFromKeys != sol::nil) {
			if (LuaCheckType(RemoveFromKeys, L_Bool, "Table:RemoveKey", { Table, Key, RemoveFromKeys }, L)) {
				RFK = ObjectToBool(RemoveFromKeys);
			}
		}
		if (LuaCheckType(Table, L_Table, "Table:RemoveKey", { Table, Key, RemoveFromKeys }, L)) {
			sol::table T = ObjectToTable(Table);
			if (RFK) {
				if (T[Key] != sol::nil) {
					T[Key] = sol::nil;
				}
				else {
					LuaError("It is not possible to delete an element from the table by key [" + ObjectToString(Key) + " (" + LUA_TypeOf(Key) + ")] because it was not found!" + LuaComplexFunction("Table:RemoveKey", { Table, Key, RemoveFromKeys }), L);
				}
			}
			else {
				if (LuaCheckType(Key, L_Int, "Table:RemoveKey", { Table, Key, RemoveFromKeys }, L)) {
					int K = ObjectToInt(Key);
					if (K < 1 || K > T.size()) {
						LuaError("Unable to delete element from table because index [" + std::to_string(K) + "] is outside table bounds [1-" + std::to_string(T.size()) + "]! " + LuaComplexFunction("Table:RemoveKey", { Table, Key, RemoveFromKeys }), L);
					}
					else {
						for (size_t i = K; i < T.size(); i++) {
							T[i] = T[i + 1];
						}
						T[T.size()] = sol::nil;
					}
				}
			}
		}
	}

	/* Удаление элемента из таблицы */
	void Remove(sol::object Table, const sol::object& Value, sol::this_state s) {
		lua_State* L = s;

		if (Value == sol::nil) {
			LuaError("The variable cannot be nil! " + LuaComplexFunction("Table:Remove", { Table, Value }), L);
			return;
		}

		if (LuaCheckType(Table, L_Table, "Table:Remove", { Table, Value }, L)) {
			sol::table T = ObjectToTable(Table);
			bool Found = false;
			size_t size = T.size();

			for (size_t i = 1; i <= size; i++) {
				if (T[i] == Value && !Found) {
					Found = true;
				}

				if (Found && i < size) {
					T[i] = T[i + 1];
				}
			}

			if (Found) {
				T[size] = sol::nil;
			}
		}
	}

	/* Добавить элемент в таблицу */
	void Add(sol::object Table, const sol::object& Value, sol::this_state s) {
		lua_State* L = s;

		if (Value == sol::nil) {
			LuaError("The variable cannot be nil! " + LuaComplexFunction("Table:Add", {Table, Value}), L);
			return;
		}

		if (LuaCheckType(Table, L_Table, "Table:Add", { Table, Value }, L)) {
			sol::table T = ObjectToTable(Table);
			T[T.size() + 1] = Value;
		}
	}

	/* Превратить таблицу в строку */
	std::string ToString(const sol::object& Table, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Table, L_Table, "Talbe:ToString", { Table }, s)) {
			sol::table T = ObjectToTable(Table);
			return TableToString(T, 0);
		}
		return ErrorString;
	}

	/* Разобрать массив */
	void Pairs(const sol::object& Table, const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Table, L_Table, "Talbe:Pairs", { Table, Func }, s)) {
			if (LuaCheckType(Func, L_Func, "Table:Pairs", { Table, Func }, s)) {
				sol::table T = ObjectToTable(Table);
				sol::function F = ObjectToFunc(Func);
				int i = 1;
				for (auto& p : T) {
					F(i, p.first, p.second);
					i++;
				}
			}
		}
	}

	/* Разобрать массив (в обратную сторону) */
	void PairsInvert(const sol::object& Table, const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Table, L_Table, "Talbe:PairsInvert", { Table, Func }, s)) {
			if (LuaCheckType(Func, L_Func, "Table:PairsInvert", { Table, Func }, s)) {
				sol::table T = ObjectToTable(Table);
				sol::function F = ObjectToFunc(Func);
				std::vector<std::tuple<int, sol::object, sol::object>> T_ = {};
				int i = 1;
				for (auto& p : T) {
					T_.push_back({i, p.first, p.second});
					i++;
				}
				std::reverse(T_.begin(), T_.end());
				for (auto& t : T_) {
					F(std::get<0>(t), std::get<1>(t), std::get<2>(t));
				}
			}
		}
	}

	/* Получить последний элемент */
	void GetLast(const sol::object& Table, const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Table, L_Table, "Talbe:GetLast", { Table, Func }, s)) {
			if (LuaCheckType(Func, L_Func, "Table:GetLast", { Table, Func }, s)) {
				sol::table T = ObjectToTable(Table);
				sol::function F = ObjectToFunc(Func);
				sol::object K;
				sol::object V;
				bool HasAnything = false;
				for (auto& p : T) {
					K = p.first;
					V = p.second;
					HasAnything = true;
				}
				if (HasAnything) {
					F(T.size() + 1, K, V);
				}
			}
		}
	}
};

LUA_Table LUA_Table_Instance;

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

	/* Ивент: колёсико мышки двигается */
	void MouseScroll(const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Func, L_Func, "Controls:MouseScroll", { Func }, L)) {
			LUA_Events_MouseScroll.push_back(ObjectToFunc(Func));
		}
	}

	/* Ивент: клавиша на мыши нажимается */
	void MousePressed(const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Func, L_Func, "Controls:MousePreesed", { Func }, L)) {
			LUA_Events_MousePressed.push_back(ObjectToFunc(Func));
		}
	}

	/* Ивент: клавиша на мыши отжимается */
	void MouseReleased(const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Func, L_Func, "Controls:MouseReleased", { Func }, L)) {
			LUA_Events_MouseReleased.push_back(ObjectToFunc(Func));
		}
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

	/* Подвинуть камеру с своим DeltaTime */
	void MoveCustom(const sol::object& Dir, const sol::object& DT, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Dir, L_Vec2, "Camera:MoveCustom", { Dir, DT }, L)) {
			if (LuaCheckNumber(DT, "Camera:MoveCustom", { Dir, DT }, L)) {
				LUA_Vector2 V2 = ObjectToVector2(Dir);
				Camera->MoveCamera(V2.x, V2.y, ObjectToDouble(DT));
			}
		}
	}

	/* Изменить зум камере */
	void MoveZoom(const sol::object& Zoom, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckNumber(Zoom, "Camera:MoveZoom", { Zoom }, L)) {
			Camera->MoveCameraZoom(ObjectToDouble(Zoom), DeltaTime);
		}
	}

	/* Изменить зум камере с своим DeltaTime */
	void MoveZoomCustom(const sol::object& Zoom, const sol::object& DT, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckNumber(Zoom, "Camera:MoveZoomCustom", { Zoom, DT }, L)) {
			if (LuaCheckNumber(DT, "Camera:MoveZoomCustom", { Zoom, DT }, L)) {
				Camera->MoveCameraZoom(ObjectToDouble(Zoom), ObjectToDouble(DT));
			}
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

	/* Получить зум камеры */
	double Zoom() {
		return Camera->Zoom;
	}

	/* Получить позицию камеры */
	LUA_Vector2 Position() {
		return LUA_Vector2(Camera->Position.x, Camera->Position.y);
	}

	/* Получить поворот камеры */
	double Orientation() {
		return Camera->Orientation;
	}
};

LUA_Camera LUA_Camera_Instance;

class LUA_Resources {
public:
	/* Загрузить скрипт */
	void LoadScript(const sol::object& Path, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Path, L_String, "Resources:LoadScript", { Path }, L)) {
			RunScript(ObjectToString(Path));
		}
	}

	/* Сохранить игровой объект в ресурсы */
	void SaveGameObject_(const sol::object& ID, const sol::object& Name, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(ID, L_Int, "Resources:SaveGameObject", { ID, Name }, L)) {
			if (LuaCheckType(Name, L_String, "Resources:SaveGameObject", { ID, Name }, L)) {
				SaveGameObject(ObjectToInt(ID), GetBaseFromLuaState(L), ObjectToString(Name));
			}
		}
	}

	/* Клонировать объект из ресурсов */
	int CloneGameObject_(const sol::object& Path, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckType(Path, L_String, "Resources:CloneGameObject", { Path }, L)) {
			return CloneSavedGameObject(ObjectToString(Path));
		}
		return -1;
	}
};

LUA_Resources LUA_Resources_Instance;

class LUA_GameObject {
public:
	/* Создать игровой объект */
	int Create(const sol::object& Name, const sol::object& Type, sol::this_state s) {
		lua_State* L = s;
		std::string Name_ = "New [" + GetBaseFromLuaState(L) + "] GameObject";
		if (Name != sol::nil) {
			if (LuaCheckType(Name, L_String, "GameObject:Create", { Name, Type }, L)) {
				Name_ = ObjectToString(Name);
			}
		}
		int Type_ = 0;
		if (Type != sol::nil) {
			if (LuaCheckType(Type, L_Int, "GameObject:Create", { Name, Type }, L)) {
				Type_ = ObjectToInt(Type);
			}
		}
		int OBJ = CreateGameObject(Name_, RO_Type(Type_), true);
		return OBJ;
	}

	/* Удалить объект */
	void Delete(const sol::object& ID, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:Delete", { ID }, L)) {
			DeleteGameObject(ObjectToInt(ID));
		}
	}

	/* Превратить игровой объект в текст */
	void MakeItText(const sol::object& ID, const sol::object& StartText, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:MakeItText", { ID, StartText }, L)) {
			std::string StartText_ = "New Text!";
			if (StartText != sol::nil){
				if (LuaCheckType(StartText, L_String, "GameObject:MakeItText", { ID, StartText }, L)) {
					StartText_ = ObjectToString(StartText);
				}
			}
			MakeGameObjectText(ObjectToInt(ID), StartText_);
		}
	}

	/* Изменить текст игровому объекту */
	void SetText(const sol::object& ID, const sol::object& NewText, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetText", { ID, NewText }, L)) {
			if (LuaCheckType(NewText, L_String, "GameObject:SetText", { ID, NewText }, L)) {
				SetGameObjectText(ObjectToInt(ID), ObjectToString(NewText));
			}
		}
	}

	/* Изменить позицию игровому объекту */
	void SetPosition(const sol::object& ID, const sol::object& NewPosition, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetPosition", { ID, NewPosition }, L)) {
			if (LuaCheckType(NewPosition, L_Vec2, "GameObject:SetPosition", { ID, NewPosition }, L)) {
				LUA_Vector2 V2 = ObjectToVector2(NewPosition);
				SetGameObjectPosition(ObjectToInt(ID), glm::vec2(V2.x, V2.y));
			}
		}
	}

	/* Получить позицию объекта */
	LUA_Vector2 GetPosition(const sol::object& ID, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:GetPosition", { ID }, L)) {
			glm::vec2 V2 = GetGameObject(ObjectToInt(ID), "").PositionVisual;
			return LUA_Vector2(V2.x, V2.y);
		}
		return ErrorVector2;
	}

	/* Изменить поворот игровому объекту */
	void SetOrientation(const sol::object& ID, const sol::object& NewOrientation, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetOrientation", { ID, NewOrientation }, L)) {
			if (LuaCheckNumber(NewOrientation, "GameObject:SetOrientation", { ID, NewOrientation }, L)) {
				SetGameObjectOrientation(ObjectToInt(ID), ObjectToDouble(NewOrientation));
			}
		}
	}

	/* Изменить центр игровому объекту */
	void SetCenter(const sol::object& ID, const sol::object& NewCenter, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetCenter", { ID, NewCenter }, L)) {
			if (LuaCheckType(NewCenter, L_Vec2, "GameObject:SetCenter", { ID, NewCenter }, L)) {
				LUA_Vector2 V2 = ObjectToVector2(NewCenter);
				SetGameObjectCenter(ObjectToInt(ID), glm::vec2(V2.x, V2.y));
			}
		}
	}

	/* Изменить цвет игровому объекту */
	void SetColor(const sol::object& ID, const sol::object& NewColor, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetColor", { ID, NewColor }, L)) {
			if (LuaCheckType(NewColor, L_Color, "GameObject:SetColor", { ID, NewColor }, L)) {
				LUA_Color C = ObjectToColor(NewColor);
				SetGameObjectColor(ObjectToInt(ID), glm::vec4(C.r, C.g, C.b, C.a));
			}
		}
	}

	/* Изменить размер игровому объекту */
	void SetSize(const sol::object& ID, const sol::object& NewSize, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetSize", { ID, NewSize }, L)) {
			if (LuaCheckType(NewSize, L_Vec2, "GameObject:SetSize", { ID, NewSize }, L)) {
				LUA_Vector2 V2 = ObjectToVector2(NewSize);
				SetGameObjectSize(ObjectToInt(ID), glm::vec2(V2.x, V2.y));
			}
		}
	}

	/* Установить размер, в зависимости от текстуры */
	void SetSizeFromTexture(const sol::object& ID, const sol::object& Path, const sol::object& Scale, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetSizeFromTexture", { ID, Path, Scale }, L)) {
			if (LuaCheckType(Path, L_String, "GameObject:SetSizeFromTexture", { ID, Path, Scale }, L)) {
				double S = 1;
				if (Scale != sol::nil) {
					if (LuaCheckNumber(Scale, "GameObject:SetSizeFromTexture", { ID, Path, Scale }, L)) {
						S = ObjectToDouble(Scale);
					}
				}
				SetGameObjectSizeFromTexture(ObjectToInt(ID), GetResourceID(ObjectToString(Path), ErrorTexture), S);
			}
		}
	}

	/* Изменить, объект меняет размер в зависимости от размера экрана? */
	void SetResize(const sol::object& ID, const sol::object& B, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetResize", { ID, B }, L)) {
			if (LuaCheckType(B, L_Bool, "GameObject:SetResize", { ID, B }, L)) {
				SetGameObjectResize(ObjectToInt(ID), ObjectToBool(B));
			}
		}
	}

	/* Сделать объект статичным */
	void SetStatic(const sol::object& ID, const sol::object& B, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetStatic", { ID, B }, L)) {
			if (LuaCheckType(B, L_Bool, "GameObject:SetStatic", { ID, B }, L)) {
				SetGameObjectStatic(ObjectToInt(ID), ObjectToBool(B));
			}
		}
	}

	/* Изменить слой объекта */
	void SetLayer(const sol::object& ID, const sol::object& NewLayer, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetLayer", { ID, NewLayer }, L)) {
			if (LuaCheckNumber(NewLayer, "GameObject:SetLayer", { ID, NewLayer }, L)) {
				SetGameObjectLayer(ObjectToInt(ID), ObjectToDouble(NewLayer));
			}
		}
	}

	/* Изменить текстуру игровому объекту */
	void SetTexture(const sol::object& ID, const sol::object& Path, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetTexture", { ID, Path }, L)) {
			if (LuaCheckType(Path, L_String, "GameObject:SetTexture", { ID, Path }, L)) {
				SetGameObjectTexture(ObjectToInt(ID), GetResourceID(ObjectToString(Path), ErrorTexture));
			}
		}
	}

	/* Изменить шейдер игровому объекту */
	void SetShader(const sol::object& ID, const sol::object& Path, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetShader", { ID, Path }, L)) {
			if (LuaCheckType(Path, L_String, "GameObject:SetShader", { ID, Path }, L)) {
				SetGameObjectShader(ObjectToInt(ID), GetResourceID(ObjectToString(Path), ErrorShader));
			}
		}
	}

	/* Добавить кастомные данные объекту */
	void SetData(const sol::object& ID, const sol::object& DataID, const sol::object& Data, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetData", { ID, DataID, Data }, L)) {
			if (LuaCheckType(DataID, L_Int, "GameObject:SetData", { ID, DataID, Data }, L)) {
				SetGameObjectLuaData(ObjectToInt(ID), ObjectToInt(DataID), Data);
			}
		}
	}

	/* Получить кастомные данные объекта */
	sol::object GetData(const sol::object& ID, const sol::object& DataID, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:GetData", { ID, DataID }, L)) {
			if (LuaCheckType(DataID, L_Int, "GameObject:GetData", { ID, DataID }, L)) {
				return GetGameObjectLuaData(ObjectToInt(ID), ObjectToInt(DataID));
			}
		}
		return sol::nil;
	}

	/* Получить имя объекта */
	std::string GetName(const sol::object& ID, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:GetName", { ID }, L)) {
			return GetGameObject(ObjectToInt(ID), "").Name;
		}
		return ErrorString;
	}

	/* Изменить, объект создан игроком? */
	void SetCreatedFromPlayer(const sol::object& ID, const sol::object& B, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetCreatedFromPlayer", { ID, B }, L)) {
			if (LuaCheckType(B, L_Bool, "GameObject:SetCreatedFromPlayer", { ID, B }, L)) {
				GetGameObject(ObjectToInt(ID), "").CreatedFromPlayer = ObjectToBool(B);
			}
		}
	}

	/* объект создан игроком? */
	bool GetCreatedFromPlayer(const sol::object& ID, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:GetCreatedFromPlayer", { ID }, L)) {;
			return GetGameObject(ObjectToInt(ID), "").CreatedFromPlayer;
		}
		return false;
	}

	/* Ивент: мышка нажала на объект */
	void MousePressed(const sol::object& ID, const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:MousePressed", { ID,Func }, L)) {
			if (LuaCheckType(Func, L_Func, "GameObject:MousePressed", { ID,Func }, L)) {
				GetGameObject(ObjectToInt(ID), "").ClickMouseEvent.push_back(ObjectToFunc(Func));
			}
		}
	}

	/* Ивент: мышка навела на объект */
	void MouseHover(const sol::object& ID, const sol::object& Func, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:MouseHover", { ID,Func }, L)) {
			if (LuaCheckType(Func, L_Func, "GameObject:MouseHover", { ID,Func }, L)) {
				GetGameObject(ObjectToInt(ID), "").MouseHoverEvent.push_back(ObjectToFunc(Func));
			}
		}
	}

	/* Установить коллайдер объекту */
	void SetCollider(const sol::object& ID, const sol::object& NewCollider, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckGameObject(ID, "GameObject:SetCollider", { ID, NewCollider }, L)) {
			if (LuaCheckType(NewCollider, L_Int, "GameObject:SetCollider", { ID, NewCollider }, L)) {
				SetGameObjectCollider(ObjectToInt(ID), ColliderType(ObjectToInt(NewCollider)));
			}
		}
	}
};

LUA_GameObject LUA_GameObject_Instance;

/* ==== Константы ==== */

std::unordered_map<std::string, int> Keys_Constants = { {"SPACE",32},{"APOSTROPHE",39},{"COMMA",44},{"MINUS",45},{"PERIOD",46},{"SLASH",47},{"0",48},{"1",49},{"2",50},{"3",51},{"4",52},{"5",53},{"6",54},{"7",55},{"8",56},{"9",57},{"SEMICOLON",59},{"EQUAL",61},{"A",65},{"B",66},{"C",67},{"D",68},{"E",69},{"F",70},{"G",71},{"H",72},{"I",73},{"J",74},{"K",75},{"L",76},{"M",77},{"N",78},{"O",79},{"P",80},{"Q",81},{"R",82},{"S",83},{"T",84},{"U",85},{"V",86},{"W",87},{"X",88},{"Y",89},{"Z",90},{"LEFT_BRACKET",91},{"BACKSLASH",92},{"RIGHT_BRACKET",93},{"GRAVE_ACCENT",96},{"WORLD_1",161},{"WORLD_2",162},{"ESCAPE",256},{"ENTER",257},{"TAB",258},{"BACKSPACE",259},{"INSERT",260},{"DELETE",261},{"RIGHT",262},{"LEFT",263},{"DOWN",264},{"UP",265},{"PAGE_UP",266},{"PAGE_DOWN",267},{"HOME",268},{"END",269},{"CAPS_LOCK",280},{"SCROLL_LOCK",281},{"NUM_LOCK",282},{"PRINT_SCREEN",283},{"PAUSE",284},{"F1",290},{"F2",291},{"F3",292},{"F4",293},{"F5",294},{"F6",295},{"F7",296},{"F8",297},{"F9",298},{"F10",299},{"F11",300},{"F12",301},{"F13",302},{"F14",303},{"F15",304},{"F16",305},{"F17",306},{"F18",307},{"F19",308},{"F20",309},{"F21",310},{"F22",311},{"F23",312},{"F24",313},{"F25",314},{"K0",320},{"K1",321},{"K2",322},{"K3",323},{"K4",324},{"K5",325},{"K6",326},{"K7",327},{"K8",328},{"K9",329},{"K_DECIMAL",330},{"K_DIVIDE",331},{"K_MULTIPLY",332},{"K_SUBTRACT",333},{"K_ADD",334},{"K_ENTER",335},{"K_EQUAL",336},{"LEFT_SHIFT",340},{"LEFT_CONTROL",341},{"LEFT_ALT",342},{"LEFT_SUPER",343},{"RIGHT_SHIFT",344},{"RIGHT_CONTROL",345},{"RIGHT_ALT",346},{"RIGHT_SUPER",347},{"MENU",348} };

/* ==== Инициализация ==== */

void GameLua(sol::state& LUA) {
	/* Константы */
	for (auto [KeyName, KeyID] : Keys_Constants) {
		LUA["KEY_"+KeyName] = sol::as_table(KeyID);
	}
	LUA["KEY_MOUSE_LEFT"] = sol::as_table(0);
	LUA["KEY_MOUSE_RIGHT"] = sol::as_table(1);
	LUA["KEY_MOUSE_MIDDLE"] = sol::as_table(2);

	LUA["PI"] = sol::as_table(PI);

	LUA["ScreenScale"] = sol::as_table(LUA_Vector2(10.0/3, 2.5));

	LUA["ErrorInt"] = sol::as_table(ErrorInt);
	LUA["ErrorDouble"] = sol::as_table(ErrorNumber);
	LUA["ErrorString"] = sol::as_table(ErrorString);
	LUA["ErrorVector2"] = sol::as_table(ErrorVector2);

	LUA["ErrorTexture"] = sol::as_table(ErrorTexture);
	LUA["ErrorShader"] = sol::as_table(ErrorShader);

	LUA["GO_Default"] = sol::as_table(RO_Default);
	LUA["GO_Physical"] = sol::as_table(RO_Phys);
	LUA["GO_UI"] = sol::as_table(RO_UI);

	LUA["COL_Square"] = sol::as_table(CT_Box);
	LUA["COL_Circle"] = sol::as_table(CT_Circle);

	/* Классы */
	LUA.new_usertype<LUA_Vector2>("Vector2",
		"X", &LUA_Vector2::x,
		"Y", &LUA_Vector2::y,
		"Abs", &LUA_Vector2::Abs,
		"Rotate", &LUA_Vector2::Rotate,
		"Length", &LUA_Vector2::Length,
		"ToString", &LUA_Vector2::ToString,
		sol::meta_function::addition, &LUA_Vector2::operator+,
		sol::meta_function::subtraction, &LUA_Vector2::operator-,
		sol::meta_function::multiplication, &LUA_Vector2::operator*,
		sol::meta_function::division, &LUA_Vector2::operator/,
		sol::meta_function::unary_minus, &LUA_Vector2::operator~,
		sol::meta_function::equal_to, &LUA_Vector2::operator==,
		sol::meta_function::less_than, &LUA_Vector2::operator<,
		sol::meta_function::less_than_or_equal_to, &LUA_Vector2::operator<=,
		sol::meta_function::concatenation, sol::overload(
			[](const char* A, LUA_Vector2& B) { return A + B.ToString(); },
			[](LUA_Vector2& B, const char* A) { return B.ToString() + A; }
		)
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

	LUA.new_usertype<LUA_Color>("Color",
		"R", &LUA_Color::r,
		"G", &LUA_Color::g,
		"B", &LUA_Color::b,
		"A", &LUA_Color::a,
		"ToString", &LUA_Color::ToString,
		sol::meta_function::concatenation, sol::overload(
			[](const char* A, LUA_Color& B) { return A + B.ToString(); },
			[](LUA_Color& B, const char* A) { return B.ToString() + A; }
		)
	);
	LUA.set_function("Color", [](sol::object r, sol::object g, sol::object b, sol::object a, sol::this_state s) {
		lua_State* L = s;
		double R = 0;
		double G = 0;
		double B = 0;
		double A = 1;
		if (r != sol::nil) {
			if (LuaCheckNumber(r, "Color", { r,g,b,a }, L)) {
				double r_ = ObjectToDouble(r);
				if (r_ < 0 || r_ > 1) {
					LuaError("$$rRed$$R in color should not go beyond $$_0$$R and $$_1$$R!$$_ Color(" + ObjectToString(r) + "," + ObjectToString(g) + "," + ObjectToString(b) + "," + ObjectToString(a) + ");", L);
				}
				else {
					R = r_;
				}
			}
		}
		if (g != sol::nil) {
			if (LuaCheckNumber(g, "Color", { r,g,b,a }, L)) {
				double g_ = ObjectToDouble(g);
				if (g_ < 0 || g_ > 1) {
					LuaError("$$GGreen$$R in color should not go beyond $$_0$$R and $$_1$$R!$$_ Color(" + ObjectToString(r) + "," + ObjectToString(g) + "," + ObjectToString(b) + "," + ObjectToString(a) + ");", L);
				}
				else {
					G = g_;
				}
			}
		}
		if (b != sol::nil) {
			if (LuaCheckNumber(b, "Color", { r,g,b,a }, L)) {
				double b_ = ObjectToDouble(b);
				if (b_ < 0 || b_ > 1) {
					LuaError("$$BBlue$$R in color should not go beyond $$_0$$R and $$_1$$R!$$_ Color(" + ObjectToString(r) + "," + ObjectToString(g) + "," + ObjectToString(b) + "," + ObjectToString(a) + ");", L);
				}
				else {
					B = b_;
				}
			}
		}
		if (a != sol::nil) {
			if (LuaCheckNumber(a, "Color", { r,g,b,a }, L)) {
				double a_ = ObjectToDouble(a);
				if (a_ < 0 || a_ > 1) {
					LuaError("$$WAlpha$$R in color should not go beyond $$_0$$R and $$_1$$R!$$_ Color(" + ObjectToString(r) + "," + ObjectToString(g) + "," + ObjectToString(b) + "," + ObjectToString(a) + ");", L);
				}
				else {
					A = a_;
				}
			}
		}
		return LUA_Color(R, G, B, A);
	});

	/* Функции*/
	LUA["OS"] = &LUA_OS_Instance;
	LUA.new_usertype<LUA_OS>(
		"LUA_OS",
		"GetSystemLanguage", &LUA_OS::GetSystemLanguage
	);

	LUA["Game"] = &LUA_Game_Instance;
	LUA.new_usertype<LUA_Game>(
		"LUA_Game",
		"Update", &LUA_Game::Update,
		"UILoading", &LUA_Game::UILoading,
		"GetFullVersion", &LUA_Game::GetFullVersion,
		"GameObjectDeleted", &LUA_Game::GameObjectDeleted,
		"GameObjectLoading", &LUA_Game::GameObjectLoading,
		"SetSimulationSpeed", &LUA_Game::SetSimulationSpeed_,
		"UpdateEveryGameObject", &LUA_Game::UpdateEveryGameObject
	);

	LUA["Controls"] = &LUA_Controls_Instance;
	LUA.new_usertype<LUA_Controls>(
		"LUA_Controls",
        "KeyPress", &LUA_Controls::KeyPress,
        "KeyPressed", &LUA_Controls::KeysPressed,
		"MouseScroll", &LUA_Controls::MouseScroll,
		"KeyReleased", &LUA_Controls::KeysReleased,
		"KeyIsPressed", &LUA_Controls::KeyIsPressed,
		"MousePressed", &LUA_Controls::MousePressed,
		"MouseReleased", &LUA_Controls::MouseReleased,
		"KeyPressedSingle", &LUA_Controls::KeyPressed_,
		"KeyReleasedSingle", &LUA_Controls::KeyReleased
	);

	LUA["Camera"] = &LUA_Camera_Instance;
	LUA.new_usertype<LUA_Camera>(
		"LUA_Camera",
		"Move", &LUA_Camera::Move,
		"Zoom", &LUA_Camera::Zoom,
		"Reset", &LUA_Camera::Reset,
		"Rotate", &LUA_Camera::Rotate,
		"SetZoom", &LUA_Camera::SetZoom,
		"Position", &LUA_Camera::Position,
		"MoveZoom", &LUA_Camera::MoveZoom,
		"MoveCustom", &LUA_Camera::MoveCustom,
		"SetPosition", &LUA_Camera::SetPosition,
		"Orientation", &LUA_Camera::Orientation,
		"SetOrientation", &LUA_Camera::SetOrientation,
		"MoveZoomCustom", &LUA_Camera::MoveZoomCustom
	);

	LUA["Table"] = &LUA_Table_Instance;
	LUA.new_usertype<LUA_Table>(
		"LUA_Table",
		"Add", &LUA_Table::Add,
		"Pairs", &LUA_Table::Pairs,
		"Remove", &LUA_Table::Remove,
		"GetLast", &LUA_Table::GetLast,
		"ToString", &LUA_Table::ToString,
		"RemoveKey", &LUA_Table::RemoveKey,
		"PairsInvert", &LUA_Table::PairsInvert
	);

	LUA["Resources"] = &LUA_Resources_Instance;
	LUA.new_usertype<LUA_Resources>(
		"LUA_Resources",
		"LoadScript", &LUA_Resources::LoadScript,
		"SaveGameObject", &LUA_Resources::SaveGameObject_,
		"CloneGameObject", &LUA_Resources::CloneGameObject_
	);

	LUA["GameObject"] = &LUA_GameObject_Instance;
	LUA.new_usertype<LUA_GameObject>(
		"LUA_GameObject",
		"Create", &LUA_GameObject::Create,
		"Delete", &LUA_GameObject::Delete,
		"SetText", &LUA_GameObject::SetText,
		"SetData", &LUA_GameObject::SetData,
		"GetData", &LUA_GameObject::GetData,
		"SetSize", &LUA_GameObject::SetSize,
		"GetName", &LUA_GameObject::GetName,
		"SetColor", &LUA_GameObject::SetColor,
		"SetLayer", &LUA_GameObject::SetLayer,
		"SetShader", &LUA_GameObject::SetShader,
		"SetCenter", &LUA_GameObject::SetCenter,
		"SetResize", &LUA_GameObject::SetResize,
		"SetStatic", &LUA_GameObject::SetStatic,
		"SetTexture", &LUA_GameObject::SetTexture,
		"MakeItText", &LUA_GameObject::MakeItText,
		"MouseHover", &LUA_GameObject::MouseHover,
		"SetPosition", &LUA_GameObject::SetPosition,
		"GetPosition", &LUA_GameObject::GetPosition,
		"SetCollider", &LUA_GameObject::SetCollider,
		"MousePressed", &LUA_GameObject::MousePressed,
		"SetOrientation", &LUA_GameObject::SetOrientation,
		"SetSizeFromTexture", &LUA_GameObject::SetSizeFromTexture,
		"SetCreatedFromPlayer", &LUA_GameObject::SetCreatedFromPlayer,
		"GetCreatedFromPlayer", &LUA_GameObject::GetCreatedFromPlayer
	);

	/* Локальные функции */
	LUA.set_function("Abs", &LUA_Abs);
	LUA.set_function("Sin", &LUA_Sin);
	LUA.set_function("Cos", &LUA_Cos);
	LUA.set_function("Rad", &LUA_Rad);
	LUA.set_function("Deg", &LUA_Deg);
	LUA.set_function("Max", &LUA_Max);
	LUA.set_function("Min", &LUA_Min);
	LUA.set_function("Sgn", &LUA_Sgn);
	LUA.set_function("Sqrt", &LUA_Sqrt);
	LUA.set_function("Cbrt", &LUA_Cbrt);
	LUA.set_function("Ceil", &LUA_Ceil);
	LUA.set_function("Floor", &LUA_Floor);
	LUA.set_function("Round", &LUA_Round);
	LUA.set_function("Print", &LUA_Print);
	LUA.set_function("Trunc", &LUA_Trunc);
	LUA.set_function("IfThen", &LUA_IfThen);
	LUA.set_function("TypeOf", &LUA_TypeOf);
	LUA.set_function("ToString", &LUA_ToString);
	LUA.set_function("DeltaTime", &LUA_DeltaTime);
	LUA.set_function("PrintFast", &LUA_PrintFast);
	LUA.set_function("RandomFast", &LUA_RandomFast);
	LUA.set_function("RoundLower", &LUA_RoundLower);
	LUA.set_function("MousePosition", &LUA_MousePosition);
	LUA.set_function("MouseLocalPosition", &LUA_MouseLocalPosition);
	LUA.set_function("MouseWorldPosition", &LUA_MouseWorldPosition);
	LUA.set_function("ScreenToWorldPosition", &LUA_ScreenToWorldPosition);
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
	LUA_Events_Update                = {};
	LUA_Events_UpdateEveryGameObject = {};
	LUA_Events_GameObjectDeleted     = {};
	LUA_Events_MouseScroll           = {};
	LUA_Events_MousePressed          = {};
	LUA_Events_MouseReleased         = {};
    LUA_Events_KeyPress              = {};
    LUA_Events_KeyPressed            = {};
    LUA_Events_KeyReleased           = {};
    LUA_Events_KeysPressed           = {};
    LUA_Events_KeysReleased          = {};
	LUA_Events_GameObjectLoading     = {};
	LUA_Events_UILoading             = {};
	ModsLUA.clear();
}

void InstallLua() {
	Print("LUA", "Lua 5.4.2");
}

const std::string WhatTheFuckLuaError = "There was an error getting Lua error, please report it to the author Lithium Universe";
void RunScript(const std::string& ScriptPath) {
	std::string Path = ComplexToFullPath(ScriptPath);
	std::string Script = ReadFile(Path);
	std::string ModID = GetBaseFromPath(ScriptPath);

	auto it = ModsLUA.find(ModID);
	if (it != ModsLUA.end()) {
		sol::state& LUA = *(it->second);
		sol::protected_function_result Result = LUA.safe_script(Script, &sol::script_pass_on_error);
		if (!Result.valid()) {
			sol::error ErrorLua = Result;
			std::string What = std::string(ErrorLua.what());
			Error(ModID, "$$RError in script $$_" + ScriptPath + "$$R!");

			bool SingleLineError = StringStartWith(What,"[string");

			std::regex Pattern(R"(:(\d+):)");
			std::smatch Matches;

			std::string ErrorLine_str = "-1";

			if (std::regex_search(What, Matches, Pattern)) {
				ErrorLine_str = Matches[1].str();
			}

			if (ErrorLine_str == "-1") {
				PrintImportant("LUA", WhatTheFuckLuaError + " (not found error line)");
				PrintImportant(ModID, ErrorLua.what());
				return;
			}

			int ErrorLine = -1;
			try {
				ErrorLine = std::stoi(ErrorLine_str);
			}
			catch (...) {
				PrintImportant("LUA", WhatTheFuckLuaError + " (can't convert \"" + ErrorLine_str + "\" to error line)");
				PrintImportant(ModID, ErrorLua.what());
				return;
			}
			std::string ErrorCode = GetLineFromString(Script, ErrorLine);
			if (SingleLineError) {
				Pattern = std::regex(R"(:\d+:\s*(.*))");
				if (std::regex_search(What, Matches, Pattern)) {
					What = Matches[1].str();
				}
				else {
					PrintImportant("LUA", WhatTheFuckLuaError + " (not found what after error line)");
					PrintImportant(ModID, ErrorLua.what());
					return;
				}
			}
			else {
				What = GetLineFromString(What, 1);
			}
			LuaErrorCustom(ModID, ErrorCode, ErrorLine, What);
			if (DeveloperVersion) {
				PrintImportant(ModID, ErrorLua.what());
			}
		}
	}
	else {
		Error("LUA","Failed to call the script because such a mod [" + ModID + "] does not exist! RunScript(\"" + ScriptPath + "\");");
	}
}