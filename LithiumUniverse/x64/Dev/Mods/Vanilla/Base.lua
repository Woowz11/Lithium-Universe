Resources:LoadScript("Vanilla:LoadGameObjects.lua")

--Game:GameObjectLoading(LoadVanillaGameObject)









local i = GameObject:Create("Default GameObject!", GO_Physical)

Resources:SaveGameObject(i, "TestObject")

local textures = {"1","2","3","4"}

local total = 70
local t = 1
for a = 0, total do
	i = Resources:CloneGameObject("Vanilla:TestObject")
	
	GameObject:SetTexture(i,"Vanilla:"..textures[t]..".png")
	GameObject:SetShader(i,"Vanilla:Shader.lu_shader");
	
	GameObject:SetPosition(i,Vector2((a-(total/2))*2.5,0))
	t = t + 1
	if (t>#textures) then
		t = 1
	end
end


Print("Vanilla loaded!")