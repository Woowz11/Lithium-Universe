UIOpened = false

function CreateOther()

end

function CreateInventory()
	local Inventory = GameObject:Create("Inventory", GO_UI)
	GameObject:SetSize(Inventory, ScreenScale-Vector2(0.1))
	--GameObject:
	UIOpened = true
end

function CreateHotBar()
	local HotBar = GameObject:Create("Hotbar", GO_UI)
	GameObject:SetPosition(HotBar, Vector2(0,-0.925))
	GameObject:SetSize(HotBar, Vector2(2,0.25))
	GameObject:SetColor(HotBar, Color(1,1,1,0.1))
end

function CreateGUI()
	CreateHotBar()
	--CreateInventory()
	CreateOther()
end