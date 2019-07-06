print("test lua")
if not LuaObj then 
	print('LuaObj not find') 
else 
	local obj = LuaObj.New(); 
	print(obj:Test(2, 1)); --2 * 2 + 1
end


function MakeKVS(...)
    local args = {...}
    local kvs = {}

    for i=1,#args/2 do
        local kv = {}
        local index = 2 * (i - 1) + 1
        kv.k = args[index]
        kv.v = args[index + 1]
        table.insert(kvs, kv)
    end
    
    return kvs
end

function MakeFVS(...)
    local args = {...}
    local fvs = {}

    for i=1,#args/2 do
        local fv = {}
        local index = 2 * (i - 1) + 1
        fv.f = args[index]
        fv.v = args[index + 1]
        table.insert(fvs, fv)
    end
    
    return fvs
end

require("redis")

local redis = BinarySafeRedis.New("127.0.0.1", 6379)
redis:Connect()

redis:Select(0)
local begintime = os.clock()
local ttt = 8
local cmds = 10000
for i=1,cmds do
	redis:HSET("hash" .. ttt, "fasfs"..i, "fsfa"..i)
end
local endtime1 = os.clock()
for i=1,cmds do
	redis:HGET("hash" .. ttt, "fasfs"..i)
end
local endtime2 = os.clock()
print((endtime1 - begintime).." write <-cost time-> read" .. (endtime2 - endtime1))
for i=1,cmds do
	redis:ZADD("sortset" .. ttt, MakeFVS(""..math.random(1, 100000), "f" .. i))
end
local endtime3 = os.clock()
print("cost time-> sortset2 " .. (endtime3 - endtime2))

function TestCallLua(a, b)
    return a*a+b + g
end

function __ERR_TRACKBACK__(msg)
    print("error: " .. tostring(msg))
    print( debug.traceback() )
    return msg
end

