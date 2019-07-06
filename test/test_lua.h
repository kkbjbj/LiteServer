#pragma once

#include "cstdio"
#include "unit_test_base.h"
#include "lua/lua_util.h"

class TestLuaBaseObj : public LuaWrapBase
{
public:
	TestLuaBaseObj() {}
	virtual ~TestLuaBaseObj() {}
	
};


class LuaObj : public TestLuaBaseObj
{
public:
	static const char *ClassName;
	static std::vector<WrapFuncInfo> FunctionList;
	static void Bind() 
	{
		WRAPFUNC_BIND(LuaObj, Test);
	}
public:
	LuaObj() {}
	virtual ~LuaObj() {}

	int Test(lua_State* L)
	{
		int p1 = (int)lua_tointeger(L, -2);
		int p2 = (int)lua_tointeger(L, -1) + p1 * p1;
		lua_pushinteger(L, p2);

		return 1;
	}
};
const char * LuaObj::ClassName = "LuaObj";
std::vector<WrapFuncInfo> LuaObj::FunctionList;

class TestLua : public TestBase
{
public:
	TestLua() {}
	~TestLua() {}

	virtual bool Run()
	{
		lua_State *L = luaL_newstate();
		luaL_openlibs(L);
		LuaWrapExport<LuaObj>().Export(L);
		LuaWrapExport<RedisConnectorWrap>().Export(L);
		if (luaL_dofile(L, "test.lua"))
		{
			LOG_ERROR("cannot load lua file: {0}\n", lua_tostring(L, -1));
		}

		int r;
		int code = LuaUtil::CallLuaFuction(L,"TestCallLua", "ii>i", 2, 1, &r);
		if(0 != code)
		{
			LOG_ERROR("CallLuaFuction Error return Code {0}\n", code);
		}
		else
		{
			LOG_ERROR("TestCallLua 2 * 2 + 1 = {0} \n", r);
		}	
		
		lua_close(L);
		return true;
	}

private:

};