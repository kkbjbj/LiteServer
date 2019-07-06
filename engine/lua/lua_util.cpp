#include <string.h>
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

int LuaUtil::CallLuaFuction(lua_State *L, const char *func, const char *sig, ... )
{
	va_list vl;
	va_start(vl, sig);
	int succ = Call(L, func,sig,vl);
	va_end(vl);

	return succ;
}

int LuaUtil::Call(lua_State *L, const char *func, const char *sig, va_list &vl)
{
	int narg, nres;	/* number of arguments and results */

	//va_start(vl, sig);
	//////////////////////////////////////////////////////////////////////////
	int top = lua_gettop(L);
	lua_getglobal(L, func);
	if (!lua_isfunction(L, -1))
	{
		printf("%s not function\n", func);
		lua_pop(L, top);
		return 1;
	}

	/* push arguments */
	narg = 0;
	while (*sig) {	/* push arguments */
		char cSig = *sig++;
		switch (cSig) {
		case  'f':
			lua_pushnumber(L, va_arg(vl, double));
			break;
		case 'd':	/* double argument */
			lua_pushnumber(L, va_arg(vl, double));
			break;
		case 'i':	/* int argument */
			lua_pushnumber(L, va_arg(vl, int));
			break;
		case 'b': /* boolean argument */
			lua_pushboolean(L, va_arg(vl, int));
			break;
		case 's':	/* string argument */
			lua_pushstring(L, va_arg(vl, char *));
			break;
		case 'p':	/* pointer argument */
			lua_pushlightuserdata(L, va_arg(vl, void *));
			break;
		case 'w':	/* wrap object argument */
		{
			LuaWrapBase* pData = (LuaWrapBase*)va_arg(vl, void *);
			LuaWrapBase** p = (LuaWrapBase**)lua_newuserdata(L, sizeof(LuaWrapBase*));
			*p = pData;
			if (pData->m_strWrapName.size() == 0)
				printf("CallLuaFuction: %s agrs table object not set m_strWrapName\n", func);
			luaL_getmetatable(L, pData->m_strWrapName.c_str());
			lua_setmetatable(L, -2);
		}
		break;
		case '>':
			goto endwhile;

		default:
			printf("CallLuaFuction invalid option (%c)\n", *(sig - 1));
		}
		narg++;
	}

endwhile:
	int functionIndex = -(narg + 1);
	int traceback = 0;
	lua_getglobal(L, "__ERR_TRACKBACK__");
	if (!lua_isfunction(L, -1))
	{
		lua_pop(L, 1);                                            /* L: ... func arg1 arg2 ... */
	}
	else
	{
		lua_insert(L, functionIndex - 1);                         /* L: ... G func arg1 arg2 ... */
		traceback = functionIndex - 1;
	}
	/* do the call */
	nres = (int)strlen(sig);	/* number of expected results */
	try
	{
		if (lua_pcall(L, narg, nres, traceback) != 0)	/* do the call */
		{
			if (traceback == 0)
			{
				printf("error running function `%s': %s\n",func, lua_tostring(L, -1));
				lua_pop(L, 1);
			}
			else                                                            /* L: ... G error */
			{
				lua_pop(L, 2); // remove __ERR_TRACKBACK__ and error message from stack
			}
			return 2;
		}
	}
	catch (int errid)
	{
		lua_pop(L, top);
		throw errid;
		return 3;
	}

	/* retrieve results */
	nres = -nres;		/* stack index of first result */
	while (*sig)
	{	/* get results */
		switch (*sig++)
		{
		case  'b':
			if (!lua_isboolean(L, nres))
				printf("wrong result type\n");
			*va_arg(vl, bool *) = lua_toboolean(L, nres) != 0;
			break;
		case  'f':
			if (!lua_isnumber(L, nres))
				printf("wrong result type\n");
			*va_arg(vl, float *) = (float)lua_tonumber(L, nres);
			break;
		case 'd':	/* double result */
			if (!lua_isnumber(L, nres))
				printf("wrong result type\n");
			*va_arg(vl, double *) = lua_tonumber(L, nres);
			break;

		case 'i':	/* int result */
			if (!lua_isnumber(L, nres))
				printf("wrong result type\n");
			*va_arg(vl, int *) = (int)lua_tonumber(L, nres);
			break;

		case 's':	/* string result */
			if (!lua_isstring(L, nres))
				printf("wrong result type\n");
			*va_arg(vl, const char **) = lua_tostring(L, nres);
			break;
		default:
			printf("invalid option (%c)\n", *(sig - 1));
		}
		lua_pop(L, 1);
		nres++;
	}
	if (traceback)
	{
		lua_pop(L, 1); // remove __ERR_TRACKBACK__
	}
	lua_pop(L, top);

	return 0;
}