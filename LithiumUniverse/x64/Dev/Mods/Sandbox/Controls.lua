SelectedSpawningGameObject = ""

SlowingTime = 0.1

GameSlowing = false
GamePaused = false

SpawnedGameObjects = {}

ShiftPressed = false

Game:GameObjectDeleted(function(OBJ)
	Table:Remove(SpawnedGameObjects, OBJ)
end)

function ClearAllSpawnedGameObjects()
	Table:PairsInvert(SpawnedGameObjects, function(i, Key, Value)
		GameObject:Delete(Value)
	end)
end

function DeleteLastSpawnedGameObject()
	Table:GetLast(SpawnedGameObjects, function(i, Key, Value)
		GameObject:Delete(Value)
	end)
end

function SelectSpawnGameObject(Path)
	SelectedSpawningGameObject = Path
	GameObject:SetText(Text_SelectedGameObject, Path)
end

Game:Update(function()
	if (ShiftPressed) then
		if (Controls:KeyIsPressed(KEY_E)) then
			SpawnGameObject(SelectedSpawningGameObject,true,MouseWorldPosition())
		end
		
		if (Controls:KeyIsPressed(KEY_Z)) then
			DeleteLastSpawnedGameObject()
		end
	end
end)

Controls:KeyReleased(function(Key)
	if (Key == KEY_LEFT_SHIFT) then
		ShiftPressed = false
	end
end)

Controls:KeyPressed(function(Key)
	if (Key == KEY_LEFT_SHIFT) then
		ShiftPressed = true
	end

	if (UIOpened) then return end

	if (not ShiftPressed) then
		if (Key == KEY_E) then
			SpawnGameObject(SelectedSpawningGameObject,true,MouseWorldPosition())
		end
			if (Key == KEY_Z) then
			DeleteLastSpawnedGameObject()
		end
	end
	if (Key == KEY_Q) then
		local count = 100
		if (SelectedSpawningGameObject == "Vanilla:Static" or SelectedSpawningGameObject == "Vanilla:Planet") then
			count = 0
		end
		for a = 0, count do
			SpawnGameObject(SelectedSpawningGameObject,false,MouseWorldPosition())
		end
	end
	
	if (Key == KEY_1) then
		SelectSpawnGameObject("Vanilla:1Mx1M")
	end
	if (Key == KEY_2) then
		SelectSpawnGameObject("Vanilla:HumanSize")
	end
	if (Key == KEY_3) then
		SelectSpawnGameObject("Vanilla:Box")
	end
	if (Key == KEY_4) then
		SelectSpawnGameObject("Vanilla:Salt")
	end
	if (Key == KEY_5) then
		SelectSpawnGameObject("Vanilla:Static")
	end
	if (Key == KEY_6) then
		SelectSpawnGameObject("Vanilla:Circle")
	end
	if (Key == KEY_7) then
		SelectSpawnGameObject("Vanilla:Planet")
	end
	
	if (Key == KEY_G) then
		GameSlowing = not GameSlowing
		GamePaused = false
		Game:SetSimulationSpeed(IfThen(GameSlowing,SlowingTime,1))
	end
	if (Key == KEY_SPACE) then
		GamePaused = not GamePaused
		GameSlowing = false
		Game:SetSimulationSpeed(IfThen(GamePaused,0,1))
	end
end)

function SpawnGameObject(Path, Right, Position)
	if (Path~="") then
		local OBJ = Resources:CloneGameObject(Path)
		GameObject:SetPosition(OBJ,Position)
		GameObject:SetCreatedFromPlayer(OBJ,true)
		Table:Add(SpawnedGameObjects,OBJ)
	end
end