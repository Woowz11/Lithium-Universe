Resources:LoadScript("Vanilla:LoadGameObjects.lua")

--Game:GameObjectLoading(LoadVanillaGameObject)





--[[local i = GameObject:Create("Default GameObject!", GO_Physical)

Resources:SaveGameObject(i, "TestObject")

local textures = {"1","2","3","4"}

local total = 70
local t = 1
for a = 0, total do
	i = Resources:CloneGameObject("Vanilla:TestObject")
	
	GameObject:SetTexture(i,"Vanilla:"..textures[t]..".png")
	GameObject:SetShader(i,"Vanilla:Shader.lu_shader")
	
	GameObject:SetPosition(i,Vector2((a-(total/2))*2.5,0))
	t = t + 1
	if (t>#textures) then
		t = 1
	end
end]]

--[[function test(i)
	PrintFast("NUMBER: "..i.." -------------------------")
	PrintFast("FLOOR : "..Floor(i))
	PrintFast("ROUND : "..Round(i))
	PrintFast("ROUNDL: "..RoundLower(i))
	PrintFast("CEIL  : "..Ceil(i))
end

test(0)
test(1)
test(-1)

test(0.1)
test(0.2)
test(0.3)
test(0.4)
test(0.5)
test(0.6)
test(0.7)
test(0.8)
test(0.9)
test(0.5001)
test(0.4999)
test(-0.5)
test(-0.5001)
test(-0.4999)]]

Print("Vanilla loaded!")