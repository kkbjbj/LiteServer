#pragma once
#include <string>
#include <vector>
#include "lua/lua.hpp"

class LuaWrapBase
{
public:
	std::string m_strWrapName;
	LuaWrapBase(){}
	virtual ~LuaWrapBase(){}
};

typedef int (LuaWrapBase::*LuaWrapFunc)(lua_State *L);
#define WRAPFUNC_CONVERT(FUNC) (LuaWrapFunc)(&FUNC)

struct WrapFuncInfo
{
	const char* Name;
	LuaWrapFunc Func;
};


#define WRAPFUNC_BIND(T, FuncName)\
do\
{\
	WrapFuncInfo info;\
	info.Name = #FuncName;\
	info.Func = WRAPFUNC_CONVERT(T::FuncName);\
	T::FunctionList.push_back(info);\
}while(0);

#define LUA_SET_VALUE(L, key, val, index)\
do\
{\
	lua_pushstring(L, key);\
	lua_pushvalue(L, val);\
	lua_settable(L, index);\
}while(0);

#define LUA_SET_FUNC(L, key, func, index)\
do\
{\
	lua_pushstring(L, key);\
	lua_pushcfunction(L, func);\
	lua_settable(L, index);\
}while(0);


template<class T>
class LuaWrapExport
{
public:
	LuaWrapExport();
	virtual ~LuaWrapExport();

	static int New(lua_State* L);
	static int Del(lua_State* L);
	static int Call(lua_State* L);
	static void Export(lua_State* L);
};


template<class T>
LuaWrapExport<T>::LuaWrapExport()
{
	T::Bind();
}

template<class T>
LuaWrapExport<T>::~LuaWrapExport()
{

}

template<class T>
int LuaWrapExport<T>::New(lua_State* L)
{
	T* obj = new T();
	T** p = (T**)lua_newuserdata(L, sizeof(T*));
	*p = obj;
	luaL_getmetatable(L,T::ClassName);
	lua_setmetatable(L,-2);
	return 1;
}

template<class T>
int LuaWrapExport<T>::Del(lua_State* L)
{
	T** p = (T**)luaL_checkudata(L, -1, T::ClassName);
	if(p)
	{
		delete (*p);
	}
	
	return 0;
}

template<class T>
int LuaWrapExport<T>::Call(lua_State* L)
{
	T** p = (T**)luaL_checkudata(L, 1,T::ClassName);
	int index = (int)lua_tointeger(L, lua_upvalueindex(1));

	LuaWrapBase *base = *p;
	LuaWrapFunc func = T::FunctionList[index].Func;
	return (base->*func)(L);
}

template<class T>
void LuaWrapExport<T>::Export(lua_State* L)
{
	int top = lua_gettop(L);
	lua_newtable(L);
	int class_index = lua_gettop(L);
	lua_pushvalue(L, class_index);
	lua_setglobal(L, T::ClassName);
	
	luaL_newmetatable(L,T::ClassName);
	int meta_index = lua_gettop(L);

	LUA_SET_VALUE(L, "__metatable", class_index, meta_index);
	LUA_SET_FUNC(L, "__gc", Del, meta_index);

	LUA_SET_FUNC(L, "New", New, class_index);
	LUA_SET_FUNC(L, "Del", Del, class_index);
	
	std::vector<WrapFuncInfo> &vList = T::FunctionList;
	for(size_t i = 0; i < vList.size();++i)
	{
		lua_pushstring(L, vList[i].Name);
		lua_pushinteger(L, i);
		lua_pushcclosure(L, LuaWrapExport<T>::Call, 1);
		lua_settable(L, class_index);
	}
	LUA_SET_VALUE(L,"__index", class_index, meta_index);
	lua_settop(L, top);
}

/*
usage:

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
		lua_pushinteger(L, 10);
		return 1;
	}
};
const char * LuaObj::ClassName = "LuaObj";
std::vector<WrapFuncInfo> LuaObj::FunctionList;

LuaWrapExport<LuaObj>().Export(L);

*/

