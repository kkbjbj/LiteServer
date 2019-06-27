#pragma once
#include "unit_test_base.h"

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
		luaL_dofile(L, "test.lua");
		lua_close(L);
		return true;
	}

private:

};