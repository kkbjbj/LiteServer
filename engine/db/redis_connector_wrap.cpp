#include <vector>
#include <string>

#include "redis_connector.h"
#include "redis_connector_wrap.h"
#include "lua/lua_util.h"

const char *RedisConnectorWrap::ClassName = "RedisConnectorWrap";
std::vector<WrapFuncInfo> RedisConnectorWrap::FunctionList;

int RedisConnectorWrap::Connect(lua_State *L)
{
	int top = lua_gettop(L);
	const char* host	= LuaUtil::GetString(L, LUACLASSCALLINDEX(1, top));
	int port	= LuaUtil::GetInt(L, LUACLASSCALLINDEX(2, top));
	int error = RedisConnect(host, port);
	lua_pushinteger(L, error);
	return 1;
}

int RedisConnectorWrap::Command(lua_State *L)
{
	int top = lua_gettop(L);
	std::string command= LuaUtil::GetString(L, LUACLASSCALLINDEX(1, top));
	int error = RedisCommand(command.c_str());
	if (error != 0)
	{
		return 0;
	}

	FillReply(L, m_pRedisReply);
	freeReplyObject(m_pRedisReply);
	m_pRedisReply = NULL;
	return 1;
}

//传入tab  {{len1, str1},{len2, str2}, ... } 命令即 str1 - strn 的拼接
int RedisConnectorWrap::SafeCommand(lua_State *L)
{
	int argn = 0;
	std::vector<std::string> strargs;

	int top = lua_gettop(L);
	if (!lua_istable(L, top))
	{
		return 0;
	}

	//遍历传入数组
	lua_pushnil(L);
	while (0 != lua_next(L, top))
	{
		int type = lua_type(L, -1);
		if (LUA_TTABLE != type)
		{
			lua_settop(L, 0);
			return 0;
		}

		int cur_index = 0;
		size_t strlenth = 0;
		
		int subindex = lua_gettop(L);
		lua_pushnil(L);
		while (0 != lua_next(L, subindex))
		{
			int type = lua_type(L, -1);
			if (0 == cur_index)
			{
				if (LUA_TNUMBER != type)
				{
					lua_settop(L, 0);
					return 0;
				}
				strlenth = (size_t)lua_tointeger(L, -1);
			}
			if (1 == cur_index)
			{
				if (LUA_TSTRING != type)
				{
					lua_settop(L, 0);
					return 0;
				}
				std::string param(lua_tolstring(L, -1, &strlenth), strlenth);
				strargs.push_back(param);
			}
			
			++cur_index;
			lua_pop(L, 1);
		}
		
		++argn;
		lua_pop(L, 1);  
	}

	if (strargs.size() > MAX_REDIS_ARGS) //命令过长
	{
		lua_settop(L, 0);
		return 0;
	}
	
	for (size_t i = 0; i < strargs.size(); i++)
	{
		m_Args[i] = strargs[i].data();
		m_ArgLen[i] = strargs[i].length();
	}

	int error = RedisSafeCommand((int)strargs.size(), m_Args, m_ArgLen);

	if (error != 0)
	{
		return 0;
	}

	FillReply(L, m_pRedisReply);
	freeReplyObject(m_pRedisReply);
	m_pRedisReply = NULL;
	return 1;
}

void RedisConnectorWrap::FillReply(lua_State *L,redisReply *reply)
{
	lua_newtable(L);
	int tab_index = -3;
	lua_pushinteger(L,1);
	lua_pushinteger(L,reply->type);
	lua_settable(L,tab_index);
	lua_pushinteger(L,2);
	lua_pushinteger(L,reply->integer);
	lua_settable(L,tab_index);
	lua_pushinteger(L,3);
	lua_pushinteger(L,reply->len);
	lua_settable(L,tab_index);
	lua_pushinteger(L,4);
	lua_pushlstring(L, reply->str, reply->len);
	lua_settable(L,tab_index);
	lua_pushinteger(L,5);
	lua_pushinteger(L,reply->elements);
	lua_settable(L,tab_index);

	lua_pushinteger(L,6);
	if (reply->elements > 0)
	{
		lua_newtable(L);
		for (size_t i = 0; i < reply->elements; ++i)
		{
			lua_pushinteger(L,i);
			FillReply(L, reply->element[i]);
			lua_settable(L, tab_index);
		}
	}
	else
	{
		lua_pushnil(L);
	}

	lua_settable(L,tab_index);
}