UIOpened = false

function CreateHelp()
	local Help = {
		"Remember that the game is in development!",
		"Version: "..Game:GetFullVersion(),
		"=== HELP ===",
		"1 - Select '1M x 1M box'",
		"2 - Select 'Human Size'",
		"3 - Select 'Omg, what is at?'",
		"E - Spawn selected",
		"Q - Spawn 100 selected",
		"W,S,A,D - Camera movement",
		"L. Shift - Faster movement",
		"L. Contr. - Slower movement",
		"Esc - Exit to menu",
		"Left, Right - Rotate camera",
		"+,- - Camera zoom",
		"Space - Stop time",
		"G - Slow time",
		"K7,K8,K9 - Set FPS limits",
	}
	
	for i = 1, #Help do
		local String = Help[i]
		local text = GameObject:Create(String, GO_UI)
		GameObject:MakeItText(text, String)
		GameObject:SetResize(text, false)
		GameObject:SetSize(text, Vector2(0.6,0.6))
		GameObject:SetCenter(text, Vector2(-1,1))
		GameObject:SetPosition(text, Vector2(-0.975, 0.8-(i/#Help)))
	end
end

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
	GameObject:SetCenter(HotBar, Vector2(0,-1))
	GameObject:SetPosition(HotBar, Vector2(0,-1))
	GameObject:SetSize(HotBar, Vector2(2,0.25))
	GameObject:SetColor(HotBar, Color(1,1,1,0.1))
end

function CreateGUI()
	--CreateHotBar()
	--CreateInventory()
	CreateOther()
	CreateHelp()
end