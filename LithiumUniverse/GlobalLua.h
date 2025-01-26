#pragma once
#include "sol/sol.hpp";

#include <string>

#include "StringActions.h";
#include "GameMod.h";

enum LUA_OBJ_Type {
	L_Unknown = -1,
	L_Nil     = 0,
	L_Int     = 1,
	L_Double  = 2,
	L_Bool    = 3,
	L_String  = 4,
	L_Table   = 5,
	L_Func    = 6,
	L_Vec2    = 7
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

extern std::vector<sol::function> LUA_Events_Update;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPress;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyPressed;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeyReleased;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeysPressed;
extern std::vector<LUA_Class_KeyPressEvent> LUA_Events_KeysReleased;

double ObjectToDouble(const sol::object& Obj);

void LuaErrorOperator(const std::string& Obj, const std::string Operator, const sol::object& ObjB);

LUA_OBJ_Type TypeOf(const sol::object& Obj);

std::string LUA_TypeOf(const sol::object& Obj);

void RunScript(const std::string& ScriptPath);
void LoadLua(GameMod Mod);
void UnloadLua();
void InstallLua();

/* === Классы ==== */

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

	double Length() const {
		return std::sqrt(x * x + y * y);
	}

	LUA_Vector2 Abs() const {
		return LUA_Vector2(abs(x),abs(y));
	}

	std::string ToString() const {
		return "Vector2(" + ToStringNumber(x) + "," + ToStringNumber(y) + ")";
	}
};