#pragma once

#include "hiredis/hiredis.h"
#include "lua/luawrap.h"
#include <string>

class RedisConnector : public LuaWrapBase
{
public:
	RedisConnector();
	virtual ~RedisConnector();
	
	int Free();
	int RedisConnect(const char * IP, int Port);
	int RedisCommand(const char *command);
	int RedisSafeCommand(int argc, const char **args, const size_t *arglen);

protected:
	redisContext *m_pRedisContext;
	redisReply *m_pRedisReply;
};