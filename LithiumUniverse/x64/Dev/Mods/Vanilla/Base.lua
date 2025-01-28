Resources:LoadScript("Vanilla:LoadGameObjects.lua")

--Game:GameObjectLoading(LoadVanillaGameObject)










local i = GameObject:Create("Default GameObject!", GO_Default)

Resources:SaveGameObject(i, "TestObject")

local j = Resources:CloneGameObject("Vanilla:TestObject")
j = Resources:CloneGameObject("Vanilla:TestObject")
j = Resources:CloneGameObject("Vanilla:TestObject")
j = Resources:CloneGameObject("Vanilla:TestObject")

for a = 1, 100 do
	--local n = Resources:CloneGameObject("Vanilla:TestObject") 
	--GameObject:SetPosition(n,Vector2((a-50)*2,0))
end








Print("Vanilla loaded!")