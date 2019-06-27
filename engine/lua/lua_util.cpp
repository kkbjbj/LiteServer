#include "lua_util.h"
#include "lua/luawrap.h"
#include "hiredis/hiredis.h"

int LuaUtil::GetInt(lua_State *L, int index)
{
	if (lua_isnil(L, index))
	{
		return 0;
	}

	int type = lua_type(L, index);
	if(LUA_TNUMBER != type && LUA_TBOOLEAN != type)
	{
		return 0;
	}

	int result = (int)lua_tointeger(L, index);
	return result;
}

int64 LuaUtil::GetInt64(lua_State *L, int index)
{
	if (lua_isnil(L, index))
	{
		return 0;
	}

	int type = lua_type(L, index);
	if(LUA_TNUMBER != type && LUA_TBOOLEAN != type)
	{
		return 0;
	}

	int64 result = (int64)lua_tonumber(L, index);
	return result;
}

float LuaUtil::GetFloat(lua_State *L, int index)
{
	if (lua_isnil(L, index))
	{
		return 0;
	}

	int type = lua_type(L, index);
	if(LUA_TNUMBER != type && LUA_TBOOLEAN != type)
	{
		return 0;
	}

	float result = (float)lua_tonumber(L, index);
	return result;
}

double LuaUtil::GetDouble(lua_State *L, int index)
{
	if (lua_isnil(L, index))
	{
		return 0;
	}

	int type = lua_type(L, index);
	if(LUA_TNUMBER != type && LUA_TBOOLEAN != type)
	{
		return 0;
	}

	double result = (double)lua_tonumber(L, index);
	return result;
}

bool LuaUtil::GetBool(lua_State *L, int index)
{
	if (lua_isnil(L, index))
	{
		return false;
	}

	int type = lua_type(L, index);
	if(LUA_TBOOLEAN != type)
	{
		return false;
	}

	bool result = lua_toboolean(L, index) != 0 ? true : false;
	return result;
}

const char* LuaUtil::GetString(lua_State *L, int index)
{
	if (lua_isnil(L, index))
	{
		return "";
	}

	int type = lua_type(L, index);
	if(LUA_TSTRING != type)
	{
		return "";
	}

	const char* result = lua_tostring(L, index);
	return result;
}

void* LuaUtil::GetUserData(lua_State *L, int index)
{
	if (lua_isnil(L, index))
	{
		return NULL;
	}

	int type = lua_type(L, index);
	if(LUA_TLIGHTUSERDATA != type && LUA_TUSERDATA != type)
	{
		return NULL;
	}

	void* result = lua_touserdata(L, index);
	return result;
}