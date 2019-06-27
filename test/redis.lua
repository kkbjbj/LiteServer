
--Author : Egg
--Date
--此文件由[BabeLua]插件自动生成
--备注: 本文件接口为二进制安全Redis接口 内部用table传命令参数 需RedisWrap提供SafeCommand接口

require("class")

BinarySafeRedis = class("BinarySafeRedis")

local REDIS_REPLY_TYPE_STRING = 1
local REDIS_REPLY_TYPE_ARRAY = 2
local REDIS_REPLY_TYPE_INTEGER = 3
local REDIS_REPLY_TYPE_NIL = 4
local REDIS_REPLY_TYPE_STATUS = 5
local REDIS_REPLY_TYPE_ERROR = 6

local REDIS_REPLY_INDEX_TYPE = 1
local REDIS_REPLY_INDEX_INT = 2
local REDIS_REPLY_INDEX_STRLEN = 3
local REDIS_REPLY_INDEX_STRDATA = 4
local REDIS_REPLY_INDEX_VALNUM = 5
local REDIS_REPLY_INDEX_VALLIST = 6

function BinarySafeRedis:ctor(Ip,Port)
    self.ip = Ip
    self.port = Port
    self.redis = RedisConnectorWrap.New()
end
-----------------------------------------------------------------------------
--inner 内部调用

function BinarySafeRedis:Command(args)
    if self.redis.SafeCommand then
        local new_arg = {}
        for i,v in ipairs(args) do
            local a = {string.len(v), v}
            table.insert(new_arg, a)
        end
        return self.redis:SafeCommand(new_arg)
    else
        local cmd = ""
        for i,v in ipairs(args) do
            cmd = cmd .. v .. " "
        end
        return self.redis:Command(cmd)
    end
end

function BinarySafeRedis:_CheckReply(rep)
    if not rep then 
        return false
    end

    if type(rep) ~= "table" then
        return false
    end

    local i = REDIS_REPLY_INDEX_TYPE
    if not rep[i] then 
        return false
    end

    if rep[i] == REDIS_REPLY_TYPE_STRING or rep[i] == REDIS_REPLY_TYPE_ARRAY or rep[i] == REDIS_REPLY_TYPE_INTEGER or rep[i] == REDIS_REPLY_TYPE_STATUS then
        return true
    end

    err(rep[REDIS_REPLY_INDEX_STRDATA] or "")
    return false
end

function BinarySafeRedis:_CommandBool(args)
    
    local rep = self:Command(args);
    if not self:_CheckReply(rep) then
        return false
    end

    local value = false
    if rep[REDIS_REPLY_INDEX_TYPE] == REDIS_REPLY_TYPE_STATUS then 
        value = true
    end

    if rep[REDIS_REPLY_INDEX_TYPE] == REDIS_REPLY_TYPE_INTEGER then 
        value = rep[REDIS_REPLY_INDEX_INT] == 1
    end

    return true, value
end

function BinarySafeRedis:_CommandString(args)
    local rep = self:Command(args);
    if not self:_CheckReply(rep) then
        return false
    end
    local value = rep[REDIS_REPLY_INDEX_STRDATA]
    return true, value
end

function BinarySafeRedis:_CommandInteger(args)
    local rep = self:Command(args);
    if not self:_CheckReply(rep) then
        return false
    end
    local value = rep[REDIS_REPLY_INDEX_INT]
    return true, value
end

function BinarySafeRedis:_CommandStatus(args)
    local rep = self:Command(args);
    if not self:_CheckReply(rep) then
        return false
    end
    local value = false
    if rep[REDIS_REPLY_INDEX_TYPE] == REDIS_REPLY_TYPE_STRING then 
        value = rep[REDIS_REPLY_INDEX_STRDATA] == "OK"
    end
    return true, value
end

function BinarySafeRedis:_CommandList(args)
    local rep = self:Command(args);
    if not self:_CheckReply(rep) then
        return false
    end
    local values = {}
    if rep[REDIS_REPLY_INDEX_VALLIST] then
        for i,v in ipairs(rep[REDIS_REPLY_INDEX_VALLIST]) do
            table.insert(values, v[REDIS_REPLY_INDEX_STRDATA])
        end
    end
    return true, values
end

-----------------------------------------------------------------------------
--link 连接状态

function BinarySafeRedis:CheckLink()
    local succ, result = self:Ping("A")
    if  result ~= "A" then
        self:Connect()
    end
end

function BinarySafeRedis:Connect()
    return self.redis:Connect(self.ip, self.port)
end

--切换数据库
--param: DBIndex(数据库下标)
--return: cmd_succ(bool 命令执行成功), result(bool select结果)
function BinarySafeRedis:Select(DBIndex)
    return self:_CommandBool({"SELECT", tostring(DBIndex)})
end

--测试连接状态
--param: Str(ping字符串)
--return: cmd_succ(bool 命令执行成功), result(str pong字符串)
function BinarySafeRedis:Ping(Str)
    return self:_CommandString({"PING", Str})
end

--认证
--param: Pwd(密码)
--return: cmd_succ(bool 命令执行成功), result(bool true成功)
function BinarySafeRedis:Auth(Pwd)
    return self:_CommandBool({"AUTH", Pwd})
end

-----------------------------------------------------------------------------
--data 非结构限制命令

--验证key存在
--param: Key(待验证key)
--return: cmd_succ(bool 命令执行成功), result(bool true存在)
function BinarySafeRedis:EXISTS(Key)
    return self:_CommandBool({"EXISTS", Key})
end

--删除一个key
--param: Key (待删除key)
--return: cmd_succ(bool 命令执行成功), result(bool true删除成功)
function BinarySafeRedis:DEL(Key)
    return self:_CommandBool({"DEL", Key})
end

--删除多个key
--param: Keys(待删除keys)
--return: cmd_succ(bool 命令执行成功), result(int 删除数量)
function BinarySafeRedis:DELMUTI(Keys)
    local args = {}
    table.insert(args, "DEL")
    local delstr = ""
    for i,v in ipairs(Keys) do
        table.insert(args, v)
    end
    return self:_CommandInteger(args)
end

--匹配数据库中key
--param: Pattern(匹配模式如:account:*)
--return: cmd_succ(bool 命令执行成功), result(strlist 匹配key列表)
function BinarySafeRedis:KEYS(Pattern)
    return self:_CommandList({"KEYS", Pattern})
end

--获取Key TTL
--param: Key (key)
--return: cmd_succ(bool 命令执行成功), result(int 剩余时间TTL)
function BinarySafeRedis:TTL(Key)
    return self:_CommandInteger({"TTL", Key})
end

--设置Key TTL
--param: Key (key), TTL(剩余时间TTL)
--return: cmd_succ(bool 命令执行成功), result(bool true成功)
function BinarySafeRedis:EXPIRE(Key, TTL)
    return self:_CommandBool({"EXPIRE", Key, tostring(TTL)})
end

-----------------------------------------------------------------------------
--data string结构命令

--获取Key数据
--param: Key (key)
--return: cmd_succ(bool 命令执行成功), result(str Key数据)
function BinarySafeRedis:GET(Key)
    return self:_CommandString({"GET", Key})
end

--设置Key数据
--param: Key (key), Value(str Key数据)
--return: cmd_succ(bool 命令执行成功), result(bool true成功)
function BinarySafeRedis:SET(Key, Value)
    return self:_CommandBool({"SET", Key, Value})
end

--获取多个Key数据
--param: Keys (keys)
--return: cmd_succ(bool 命令执行成功), result(strlist Key数据列表)
function BinarySafeRedis:MGET(Keys)
    local args = {}
    table.insert(args, "MGET")

    for i,v in ipairs(Keys) do
        table.insert(args, v)
    end
    return self:_CommandList(args)
end

--设置多个Key数据
--param: DataList({{k =k, v = v}})
--return: cmd_succ(bool 命令执行成功), result(bool true成功)
function BinarySafeRedis:MSET(DataList)
    local args = {}
    table.insert(args, "MSET")

    for i,v in ipairs(DataList) do
        table.insert(args, v.k)
        table.insert(args, v.v)
    end
    return self:_CommandBool(args)
end

--Key递增
--param: Key(递增key)
--return: cmd_succ(bool 命令执行成功), result(int Key值)
function BinarySafeRedis:INCR(Key)
    return self:_CommandInteger({"INCR", Key})
end

-----------------------------------------------------------------------------
--data list结构命令

--插入列表尾
--param: Key(插入 key), Values(插入数据列表)
--return: cmd_succ(bool 命令执行成功), result(int 插入数量)
function BinarySafeRedis:RPUSH(Key, Values)
    local args = {}
    table.insert(args, "RPUSH")
    table.insert(args, Key)
    for i,v in ipairs(Values) do
        table.insert(args, v)
    end
    return self:_CommandInteger(args)
end

--获取列表长度
--param: Key(key)
--return: cmd_succ(bool 命令执行成功), result(int 列表长度)
function BinarySafeRedis:LLEN(Key)
    return self:_CommandInteger({"LLEN",Key})
end

--获取列表区间
--param: Key(key), Start(开始下标), End(结束下标) 
--return: cmd_succ(bool 命令执行成功), result(strlist 区间元素)
function BinarySafeRedis:LRANGE(Key, Start, End)
    return self:_CommandList({"LRANGE", Key, tostring(Start), tostring(End)})
end

--从列表移除
--param: Key(key), Value(移除值)
--return: cmd_succ(bool 命令执行成功), result(bool true成功)
function BinarySafeRedis:LREM(Key, Value)
    return self:_CommandBool({"LREM", Key, "0", Value}) --0表示移除表中所有与 value 相等的值
end

-----------------------------------------------------------------------------
--data hash结构命令

--删除哈希表中的指定域
--param: Key(key), FiledsList(删除域)
--return:cmd_succ(bool 命令执行成功), result(int 删除数量)
function BinarySafeRedis:HDEL(Key, FiledsList)
    local args = {}
    table.insert(args, "HDEL")
    table.insert(args, Key)
    for i,v in ipairs(FiledsList) do
        table.insert(args, v)
    end
    return self:_CommandInteger(args)
end

--获取哈希表中所有域和值
--param: Key(key)
--return:cmd_succ(bool 命令执行成功), result(strlist 域值域值域值)
function BinarySafeRedis:HGETALL(Key)
    return self:_CommandList({"HGETALL", Key})
end

--获取哈希表中的指定域
--param: Key(key), Filed(查询域)
--return:cmd_succ(bool 命令执行成功), result(str 指定域数据)
function BinarySafeRedis:HGET(Key, Filed)
    return self:_CommandString({"HGET", Key, Filed})
end

--设置哈希表中的指定域值
--param: Key(key), Filed(域)
--return:cmd_succ(bool 命令执行成功), result(bool true成功)
function BinarySafeRedis:HSET(Key, Filed, Value)
    return self:_CommandBool({"HSET", Key, Filed, Value})
end


--获取哈希表中多个指定域
--param: Key(key), FiledsList(查询域列表)
--return:cmd_succ(bool 命令执行成功), result(strlist 指定域数据列表)
function BinarySafeRedis:HMGET(Key, FiledsList)
    local args = {}
    table.insert(args, "HMGET")
    table.insert(args, Key)
    for i,v in ipairs(FiledsList) do
        table.insert(args, v)
    end
    return self:_CommandList(args)
end

--设置哈希表中多个指定域值
--param: Key(key), FVList(域-值列表{{f=f,v=v}})
--return:cmd_succ(bool 命令执行成功), result(bool true成功)
function BinarySafeRedis:HMSET(Key, FVList)
    local args = {}
    table.insert(args, "HMGET")
    table.insert(args, Key)
    for i,v in ipairs(FVList) do
        table.insert(args, v.f)
        table.insert(args, v.v)
    end
    return self:_CommandBool(args)
end

--获取哈希表中域的数量
--param: Key(key)
--return:cmd_succ(bool 命令执行成功), result(int 域数量)
function BinarySafeRedis:HLEN(Key)
    return self:_CommandInteger({"HLEN", Key})
end

--获取哈希表中域列表
--param: Key(key)
--return:cmd_succ(bool 命令执行成功), result(strlist 域列表)
function BinarySafeRedis:HKEYS(Key)
    return self:_CommandList({"HKEYS",  Key})
end

--为哈希表中指定域值加上增量
--param: Key(key) Filed(域), Incr(增量)
--return:cmd_succ(bool 命令执行成功), result(int 当前域值)
function BinarySafeRedis:HINCRBY(Key, Filed, Incr)
    return self:_CommandInteger({"HINCRBY", Key, Filed, tostring(Incr)})
end

--哈希表中指定域是否存在
--param: Key(key) Filed(域)
--return:cmd_succ(bool 命令执行成功), result(bool true存在)
function BinarySafeRedis:HEXISTS(Key, Filed)
    return self:_CommandBool({"HEXISTS", Key, Filed})
end

--获取哈希表中值列表
--param: Key(key)
--return:cmd_succ(bool 命令执行成功), result(strlist 值列表)
function BinarySafeRedis:HVALS(Key)
    return self:_CommandList({"HVALS", Key})
end

-----------------------------------------------------------------------------
--data set结构命令

--元素(1-n个)加入到集合
--param: Key(集合), Values(元素列表)
--return: cmd_succ(bool 命令执行成功), result(int 添加数量)
function BinarySafeRedis:SADD(Key, Values)
    local args = {}
    table.insert(args, "SADD")
    table.insert(args, Key)
    for i,v in ipairs(Values) do
        table.insert(args, v)
    end
    return self:_CommandInteger(args)
end

--获取集合中的所有成员
--param: Key(集合)
--return: cmd_succ(bool 命令执行成功), result(strlist 集合成员)
function BinarySafeRedis:SMEMBERS(Key)
    return self:_CommandList({"SMEMBERS", Key})
end

--移除集合中的元素(1-n个)
--param: Key(集合), Values(元素列表)
--return: cmd_succ(bool 命令执行成功), result(int 移除数量)
function BinarySafeRedis:SREM(Key, Values)
    local args = {}
    table.insert(args, "SREM")
    table.insert(args, Key)
    for i,v in ipairs(Values) do
        table.insert(args, v)
    end
    return self:_CommandInteger(args)
end

--判断元素是否是集合成员
--param: Key(集合), Value(元素)
--return: cmd_succ(bool 命令执行成功), result(bool true是成员)
function BinarySafeRedis:SISMEMBER(Key, Value)
    return self:_CommandInteger({"SISMEMBER", Key, Value})
end

-----------------------------------------------------------------------------
--data sortset结构命令

--将元素加入到有序集当中
--param: Key(key), FVList(元素列表{{f=f,v=v}}) f分数 v成员
--return: cmd_succ(bool 命令执行成功), result(int ADD数量)
function BinarySafeRedis:ZADD(Key, FVList)
    local args = {}
    table.insert(args, "ZADD")
    table.insert(args, Key)
    for i,v in ipairs(FVList) do
        table.insert(args, v.f)
        table.insert(args, v.v)
    end
    return self:_CommandInteger(args)
end

--为有序集成员的score值加上增量
--param: Key(key), Incr(增量), Filed(成员)
--return: cmd_succ(bool 命令执行成功), result(int 最新score)
function BinarySafeRedis:ZINCRBY(Key, Incr, Filed)
    return self:_CommandString({"ZINCRBY", Key, tostring(Incr), Filed})
end

--移除有序集中的成员(1-n个)
--param: Key(key), FiledsList(元素列表)
--return: cmd_succ(bool 命令执行成功), result(int 移除数量)
function BinarySafeRedis:ZREM(Key, FiledsList)
    local args = {}
    table.insert(args, "ZREM")
    table.insert(args, Key)
    for i,v in ipairs(FiledsList) do
        table.insert(args, v)
    end
    return self:_CommandInteger(args)
end

--获取有序集中指定下标区间内的成员
--param: Key(key), Start(开始下标), End(结束下标) 
--return: cmd_succ(bool 命令执行成功), result(strlist 区间元素)
function BinarySafeRedis:ZRANGE(Key, Start, End)
    return self:_CommandList({"ZRANGE", Key, tostring(Start), tostring(End)})
end

--获取有序集中指定下标区间内的成员及score
--param: Key(key), Start(开始下标), End(结束下标) 
--return: cmd_succ(bool 命令执行成功), result(strlist 区间元素及score)
function BinarySafeRedis:ZRANGEWITHSCORE(Key, Start, End)
    return self:_CommandList({"ZRANGE", Key, tostring(Start), tostring(End), "WITHSCORES"})
end

--获取有序集中指定下标区间内的成员(递减)
--param: Key(key), Start(开始下标), End(结束下标) 
--return: cmd_succ(bool 命令执行成功), result(strlist 区间元素)
function BinarySafeRedis:ZREVRANGE(Key, Start, End)
    return self:_CommandList({"ZREVRANGE", Key, tostring(Start), tostring(End)})
end

--获取有序集中指定下标区间内的成员及score(递减)
--param: Key(key), Start(开始下标), End(结束下标) 
--return: cmd_succ(bool 命令执行成功), result(strlist 区间元素及score)
function BinarySafeRedis:ZRERANGEWITHSCORE(Key, Start, End)
    return self:_CommandList({"ZREVRANGE", Key, tostring(Start), tostring(End), "WITHSCORES"})
end

--获取有序集中指定Score区间内的成员(递减)
--param: Key(key), StartScore(开始Score), EndScore(结束Score) 
--return: cmd_succ(bool 命令执行成功), result(strlist 区间元素)
function BinarySafeRedis:ZREVRANGEBYSCORE(Key, StartScore, EndScore)
    return self:_CommandList({"ZREVRANGEBYSCORE", Key, tostring(StartScore), tostring(EndScore)})
end

--获取有序集中指定Score区间内的成员
--param: Key(key), StartScore(开始Score), EndScore(结束Score) 
--return: cmd_succ(bool 命令执行成功), result(strlist 区间元素)
function BinarySafeRedis:ZRANGEBYSCORE(Key, StartScore, EndScore)
    return self:_CommandList({"ZRANGEBYSCORE", Key, tostring(StartScore), tostring(EndScore)})
end

--获取有序集中指定Score区间内的成员数量
--param: Key(key), StartScore(开始Score), EndScore(结束Score) 
--return: cmd_succ(bool 命令执行成功), result(int 成员数量)
function BinarySafeRedis:ZCOUNT(Key, StartScore, EndScore)
    return self:_CommandInteger({"ZCOUNT", Key, tostring(StartScore), tostring(EndScore)})
end


--获取有序集的基数
--param: Key(key)
--return: cmd_succ(bool 命令执行成功), result(int 基数)
function BinarySafeRedis:ZCARD(Key)
    return self:_CommandInteger({"ZCARD", Key})
end

--获取有序集成员排名
--param: Key(key), Filed(成员)
--return: cmd_succ(bool 命令执行成功), result(int 成员排名)
function BinarySafeRedis:ZRANK(Key, Filed)
    return self:_CommandInteger({"ZRANK", Key, Filed})
end

--获取有序集成员排名()递减
--param: Key(key), Filed(成员)
--return: cmd_succ(bool 命令执行成功), result(int 成员排名)
function BinarySafeRedis:ZREVRANK(Key, Filed)
    return self:_CommandInteger({"ZREVRANK", Key, Filed})
end

--获取有序集成员Score
--param: Key(key), Filed(成员)
--return: cmd_succ(bool 命令执行成功), result(str 成员Score)
function BinarySafeRedis:ZSCORE(Key, Filed)
    return self:_CommandString({"ZSCORE", Key, Filed})
end


