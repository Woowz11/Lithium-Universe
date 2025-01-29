Resources:LoadScript("Vanilla:LoadGameObjects.lua")

--Game:GameObjectLoading(LoadVanillaGameObject)









local i = GameObject:Create("Default GameObject!", GO_Physical)

Resources:SaveGameObject(i, "TestObject")

i = Resources:CloneGameObject("Vanilla:TestObject")

Game:Update(function()
	GameObject:SetPosition(1,MouseWorldPosition())
end)





Print("Vanilla loaded!")