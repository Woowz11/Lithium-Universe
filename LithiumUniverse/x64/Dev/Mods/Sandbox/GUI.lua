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
	
	local HelpResult = ""
	for i = 1, #Help do
		local String = Help[i]
		HelpResult = HelpResult .. String .. "\n"
	end
	local text = GameObject:Create("Help Text", GO_UI)
	GameObject:MakeItText(text, HelpResult)
	GameObject:SetSize(text, Vector2(1.25)/ScreenScale)
	GameObject:SetCenter(text, Vector2(-1,0))
	GameObject:SetPosition(text, Vector2(-0.99, 0.5))
end

Text_SelectedGameObject = -1
function CreateOther()
	Text_SelectedGameObject = GameObject:Create("Selected GameObject to Spawn", GO_UI)
	GameObject:MakeItText(Text_SelectedGameObject, "")
	GameObject:SetResize(Text_SelectedGameObject, false)
	GameObject:SetSize(Text_SelectedGameObject, Vector2(0.7,0.7))
	GameObject:SetPosition(Text_SelectedGameObject, Vector2(0, -0.99))
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