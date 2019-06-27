#pragma once

#include "util/type_def.h"

#define LUACLASSCALLINDEX(index, argc) (index - argc)
struct lua_State;
class LuaUtil
{
public:
	LuaUtil(){}
	~LuaUtil(){}
public:
	static int GetInt(lua_State *L, int index);	
	static int64 GetInt64(lua_State *L, int index);
	static float GetFloat(lua_State *L, int index);
	static double GetDouble(lua_State *L, int index);
	static bool GetBool(lua_State *L, int index);
	static const char* GetString(lua_State *L, int index);
	static void* GetUserData(lua_State *L, int index);
};

