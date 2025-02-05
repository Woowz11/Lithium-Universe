UIOpened = false

function CreateHelp()
	local HelpEN = {
		"Remember that the game is in development!",
		"Version: "..Game:GetFullVersion(),
		"=== CONTROLS ===",
		"1 - Select '1M x 1M box'",
		"2 - Select 'Human Size'",
		"3 - Select 'Omg, what is at?'",
		"4 - Select 'Salt'",
		"5 - Select 'Static box'",
		"6 - Select 'Circle'",
		"7 - Select 'Planet'",
		"E - Spawn selected",
		"Q - Spawn 100 selected",
		"Z - Delete last spawned",
		"W,S,A,D - Camera movement",
		"L. Shift - Faster movement",
		"L. Contr. - Slower movement",
		"Home - Reset camera",
		"Esc - Exit to menu",
		"Left, Right - Rotate camera",
		"+,- - Camera zoom",
		"Space - Stop time",
		"G - Slow time",
		"K7,K8,K9 - Set FPS limits",
		"F3 - Switch debug ui",
		"F4 - Reload resources"
	}
	
	local HelpRU = {
		"Помните, что игра находится в разработке!",
		"Версия: "..Game:GetFullVersion(),
		"=== УПРАВЛЕНИЕ ===",
		"1 - Выбрать '1M x 1M box'",
		"2 - Выбрать 'Human Size'",
		"3 - Выбрать 'Omg, what is at?'",
		"4 - Выбрать 'Salt'",
		"5 - Выбрать 'Static box'",
		"6 - Выбрать 'Circle'",
		"7 - Выбрать 'Planet'",
		"E - Заспавнить выбранное",
		"Q - Заспавнить 100 раз выбранное",
		"Z - Удалить последний заспавненный объект",
		"W,S,A,D - Двигать камеру",
		"L. Shift - Двигать быстрее",
		"L. Contr. - Двигать медленее",
		"Home - Вернуть камеру в начальную позицию",
		"Esc - Выйти в меню",
		"Left, Right - Вращать камеру",
		"+,- - Масштабировать камеру",
		"Space - Остановить время",
		"G - Замедлить время",
		"K7,K8,K9 - Установить FPS лимиты",
		"F3 - Переключить отладочный интерфейс",
		"F4 - Обновить ресурсы"
	}
	
	local Help = IfThen(OS:GetSystemLanguage()=="Russian", HelpRU,HelpEN)
	
	local HelpResult = ""
	for i = 1, #Help do
		local String = Help[i]
		HelpResult = HelpResult .. String .. "\n"
	end
	local text = GameObject:Create("Help Text", GO_UI)
	GameObject:MakeItText(text, HelpResult)
	GameObject:SetSize(text, Vector2(1)/ScreenScale)
	GameObject:SetCenter(text, Vector2(-1,0))
	GameObject:SetPosition(text, Vector2(-0.99, 0.5))
end

function MakeItButton(OBJ,Event)
	GameObject:MousePressed(OBJ,function(Key)
		if (Key == KEY_MOUSE_LEFT) then
			Event()
		end
	end)
	GameObject:SetTexture(OBJ,"Base:Textures/Button.png")
	GameObject:MouseHover(OBJ,function(Hover,Orig)
		if (Hover) then
			GameObject:SetTexture(Orig,"Base:Textures/ButtonHover.png")
		else
			GameObject:SetTexture(Orig,"Base:Textures/Button.png")
		end
	end)
end

Text_SelectedGameObject = -1
function CreateOther()
	Text_SelectedGameObject = GameObject:Create("Selected GameObject to Spawn", GO_UI)
	GameObject:MakeItText(Text_SelectedGameObject, "")
	GameObject:SetResize(Text_SelectedGameObject, false)
	GameObject:SetSize(Text_SelectedGameObject, Vector2(0.7,0.7))
	GameObject:SetPosition(Text_SelectedGameObject, Vector2(0, -0.99))
	
	local ClearButton = GameObject:Create("Clear Button", GO_UI)
	MakeItButton(ClearButton,function() ClearAllSpawnedGameObjects() end)
	GameObject:SetSize(ClearButton,Vector2(0.25,0.25))
	GameObject:SetCenter(ClearButton,Vector2(-1,-1))
	GameObject:SetPosition(ClearButton,Vector2(-0.99,-0.99))
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