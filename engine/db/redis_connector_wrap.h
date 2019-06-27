#pragma once

#include "redis_connector.h"

#define MAX_REDIS_ARGS 1024

class RedisConnectorWrap : public RedisConnector
{

public:
	RedisConnectorWrap(){}
	~RedisConnectorWrap(){}

public:
	static const char *ClassName;
	static std::vector<WrapFuncInfo> FunctionList;
	static void Bind() 
	{
		WRAPFUNC_BIND(RedisConnectorWrap, Connect);
		WRAPFUNC_BIND(RedisConnectorWrap, Command);
		WRAPFUNC_BIND(RedisConnectorWrap, SafeCommand);
	}

public:
	int Connect(lua_State* L);
	int Command(lua_State* L);
	int SafeCommand(lua_State* L);
private:
	void FillReply(lua_State *L,redisReply *rr);
	const char *m_Args[MAX_REDIS_ARGS];
	size_t m_ArgLen[MAX_REDIS_ARGS];
};