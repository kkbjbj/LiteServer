print("test lua")
if not LuaObj then 
	print('LuaObj not find') 
else 
	local obj = LuaObj.New(); 
	print(obj:Test(2, 1)); --2 * 2 + 1
end

require("redis")

local redis = BinarySafeRedis.New("127.0.0.1", 6379)
redis:Connect()

