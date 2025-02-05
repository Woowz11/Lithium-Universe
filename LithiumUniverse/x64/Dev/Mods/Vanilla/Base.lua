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

local Salt = Resources:CloneGameObject("Vanilla:1Mx1M")
GameObject:SetTexture(Salt, "Vanilla:salt.png")
GameObject:SetSize(Salt, Vector2(0.01))
Resources:SaveGameObject(Salt, "Salt")

local Static = Resources:CloneGameObject("Vanilla:1Mx1M")
GameObject:SetTexture(Static, "Base:Textures/Blank.png")
GameObject:SetStatic(Static, true)
GameObject:SetColor(Static, Color(0.2,0.2,0.2))
Resources:SaveGameObject(Static, "Static")

local Circle = Resources:CloneGameObject("Vanilla:1Mx1M")
GameObject:SetTexture(Circle, "Vanilla:Circle.png")
GameObject:SetCollider(Circle, COL_Circle)
Resources:SaveGameObject(Circle, "Circle")

local Planet = Resources:CloneGameObject("Vanilla:Circle")
GameObject:SetTexture(Planet, "Vanilla:Planet.png")
GameObject:SetSizeFromTexture(Planet, "Vanilla:Planet.png", 50)
GameObject:SetStatic(Planet, true)
GameObject:SetLayer(Planet, -10)
Resources:SaveGameObject(Planet, "Planet")

Print("Vanilla loaded!")