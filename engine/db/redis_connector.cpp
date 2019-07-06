#include "redis_connector.h"

RedisConnector::RedisConnector()
:m_pRedisContext(NULL)
,m_pRedisReply(NULL)
{
	m_strWrapName = "RedisConnectorWrap";
}

RedisConnector::~RedisConnector()
{
	Free();
}

int RedisConnector::Free()
{
	if (NULL != m_pRedisContext)
	{
		redisFree(m_pRedisContext);
	}

	if (NULL != m_pRedisReply)
	{
		freeReplyObject(m_pRedisReply);
	}

	m_pRedisContext = NULL;
	m_pRedisReply = NULL;

	return 0;
}

int RedisConnector::RedisConnect(const char * IP, int Port)
{
	Free();
	m_pRedisContext = redisConnect(IP, Port);
	int err = m_pRedisContext->err;
	if (err)
	{
		printf("Connect redis faile:%s\n", m_pRedisContext->errstr);
		Free();
		return err;
	}
	printf("Connect redis succ\n");
	return err;
}

int RedisConnector::RedisCommand(const char *command)
{
	if (NULL == m_pRedisContext)
	{
		printf("redis not connect\n");
		return 1;
	}

	m_pRedisReply = (redisReply*)redisCommand(m_pRedisContext, command);
	if (NULL == m_pRedisReply)
	{
		return 1;
	}

	return 0;
}

int RedisConnector::RedisSafeCommand(int argc, const char **args, const size_t *arglen)
{
	if (NULL == m_pRedisContext)
	{
		printf("redis not connect\n");
		return 1;
	}

	m_pRedisReply = (redisReply*)redisCommandArgv(m_pRedisContext, argc, args, arglen);
	if (NULL == m_pRedisReply)
	{
		return 1;
	}

	return 0;
}
