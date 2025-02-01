function CreateOther()

end

function CreateInventory()

end

function CreateHotBar()
	local HotBar = GameObject:Create("Hotbar", GO_UI)
	GameObject:SetPosition(HotBar, Vector2(0,-0.925))
	GameObject:SetSize(HotBar, Vector2(2,0.25))
	GameObject:SetColor(HotBar, Color(1,1,1,0.1))
end

function CreateGUI()
	CreateInventory()
	CreateHotBar()
	CreateOther()
end