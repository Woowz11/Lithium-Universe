Resources:LoadScript("Vanilla:LoadGameObjects.lua")

--Game:GameObjectLoading(LoadVanillaGameObject)

local HumanSize = GameObject:Create("HumanSize", GO_Physical)
GameObject:SetTexture(HumanSize, "Vanilla:HumanSize.png")
GameObject:SetSizeFromTexture(HumanSize, "Vanilla:HumanSize.png")
Resources:SaveGameObject(HumanSize, "HumanSize")

local MxM = GameObject:Create("1Mx1M", GO_Physical)
GameObject:SetTexture(MxM, "Vanilla:1Mx1M.png")
GameObject:SetSizeFromTexture(MxM, "Vanilla:1Mx1M.png")
Resources:SaveGameObject(MxM, "1Mx1M")

local Box = Resources:CloneGameObject("Vanilla:1Mx1M")
GameObject:SetTexture(Box, "Vanilla:1.png")
Resources:SaveGameObject(Box, "Box")

Print("Vanilla loaded!")