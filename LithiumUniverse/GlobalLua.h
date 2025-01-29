#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include <GLM/glm.hpp>
#include <GLM/gtx/rotate_vector.hpp>
#include <GLM/gtx/vector_angle.hpp>

#include "sol/sol.hpp";

#include <string>

#include "StringActions.h";
#include "GameMod.h";

#include "Console.h";

enum LUA_OBJ_Type {
	L_Unknown = -1,
	L_Nil     = 0,
	L_Int     = 1,
	L_Double  = 2,
	L_Bool    = 3,
	L_String  = 4,
	L_Table   = 5,
	L_Func    = 6,
	L_Vec2    = 7,
	L_Color   = 8
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

extern const double ErrorNumber;
extern const int ErrorInt;
extern const std::string ErrorString;

extern std::vector<sol::function> LUA_Events_Update;
extern std::vector<sol::function> LUA_Events_GameObjectLoading;
extern std::vector<sol::function> LUA_Events_MouseScroll;
extern std::vector<sol::function> LUA_Events_MousePressed;
extern std::vector<sol::function> LUA_Events_MouseReleased;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPress;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPressed;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyReleased;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeysPressed;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeysReleased;

double ObjectToDouble(const sol::object& Obj);

void LuaErrorOperator(const std::string& Obj, const std::string Operator, const sol::object& ObjB);
bool LuaCheckNumber(const sol::object& Obj, const std::string Function, const std::vector<sol::object>& Params, lua_State* L);

std::string TableToString(const sol::table& Table, int Hierarchy);
LUA_OBJ_Type TypeOf(const sol::object& Obj);

std::string LUA_TypeOf(const sol::object& Obj);

void RunScript(const std::string& ScriptPath);
void LoadLua(GameMod Mod);
void UnloadLua();
void InstallLua();

/* === Классы ==== */

class LUA_Color {
public:
	double r, g, b, a;
	LUA_Color(double r, double g, double b, double a) : r(r), g(g), b(b), a(a) {}

	std::string ToString() const {
		if (a == 1) {
			return "Color(" + ToStringNumber(r) + "," + ToStringNumber(g) + "," + ToStringNumber(b) + ")";
		}
		else {
			return "Color(" + ToStringNumber(r) + "," + ToStringNumber(g) + "," + ToStringNumber(b) + "," + ToStringNumber(a) + ")";
		}
	}
};

class LUA_Vector2 {
public:
	double x, y;
	LUA_Vector2(double x, double y) : x(x), y(y) {}

	LUA_Vector2 operator+(const sol::object& Other) const {
		switch (TypeOf(Other))
		{
		case L_Vec2: {
			LUA_Vector2 Vector2Other = Other.as<LUA_Vector2>();
			return LUA_Vector2(x + Vector2Other.x, y + Vector2Other.y);
			break;
		}
		case L_Int:
		case L_Double: {
			double D = ObjectToDouble(Other);
			return LUA_Vector2(x + D, y + D);
			break;
		}
		default:
			LuaErrorOperator(ToString(), "+", Other);
			return LUA_Vector2(ErrorNumber, ErrorNumber);
			break;
		}
	}

	LUA_Vector2 operator-(const sol::object& Other) const {
		switch (TypeOf(Other))
		{
		case L_Vec2: {
			LUA_Vector2 Vector2Other = Other.as<LUA_Vector2>();
			return LUA_Vector2(x - Vector2Other.x, y - Vector2Other.y);
			break;
		}
		case L_Int:
		case L_Double: {
			double D = ObjectToDouble(Other);
			return LUA_Vector2(x - D, y - D);
			break;
		}
		default:
			LuaErrorOperator(ToString(), "-", Other);
			return LUA_Vector2(ErrorNumber, ErrorNumber);
			break;
		}
	}

	LUA_Vector2 operator*(const sol::object& Other) const {
		switch (TypeOf(Other))
		{
		case L_Vec2: {
			LUA_Vector2 Vector2Other = Other.as<LUA_Vector2>();
			return LUA_Vector2(x * Vector2Other.x, y * Vector2Other.y);
			break;
		}
		case L_Int:
		case L_Double: {
			double D = ObjectToDouble(Other);
			return LUA_Vector2(x * D, y * D);
			break;
		}
		default:
			LuaErrorOperator(ToString(), "*", Other);
			return LUA_Vector2(ErrorNumber, ErrorNumber);
			break;
		}
	}

	LUA_Vector2 operator/(const sol::object& Other) const {
		switch (TypeOf(Other))
		{
		case L_Vec2: {
			LUA_Vector2 Vector2Other = Other.as<LUA_Vector2>();
			return LUA_Vector2(x / Vector2Other.x, y / Vector2Other.y);
			break;
		}
		case L_Int:
		case L_Double: {
			double D = ObjectToDouble(Other);
			return LUA_Vector2(x / D, y / D);
			break;
		}
		default:
			LuaErrorOperator(ToString(), "/", Other);
			return LUA_Vector2(ErrorNumber, ErrorNumber);
			break;
		}
	}

	bool operator==(const LUA_Vector2& Other) const {
		return x == Other.x && y == Other.y;
	}

	bool operator<(const LUA_Vector2& Other) const {
		return x < Other.x && y < Other.y;
	}

	bool operator<=(const LUA_Vector2& Other) const {
		return x <= Other.x && y <= Other.y;
	}

	LUA_Vector2 operator~() const {
		return LUA_Vector2(-x, -y);
	}

	double Length() const {
		return std::sqrt(x * x + y * y);
	}

	LUA_Vector2 Abs() const {
		return LUA_Vector2(abs(x),abs(y));
	}

	LUA_Vector2 Rotate(const sol::object& Angle, sol::this_state s) {
		lua_State* L = s;
		if (LuaCheckNumber(Angle, ToString() + ":Rotate", {Angle}, L)) {
			glm::vec2 V2 = glm::vec2(x, y);
			V2 = glm::rotate(V2, (float)ObjectToDouble(Angle));
			return LUA_Vector2(V2.x, V2.y);
		}
		return LUA_Vector2(ErrorNumber, ErrorNumber);
	}

	std::string ToString() const {
		return "Vector2(" + ToStringNumber(x) + "," + ToStringNumber(y) + ")";
	}
};

extern const LUA_Vector2 ErrorVector2;