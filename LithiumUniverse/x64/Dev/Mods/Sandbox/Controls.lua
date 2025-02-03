SelectedSpawningGameObject = ""

SlowingTime = 0.1

GameSlowing = false
GamePaused = false

SpawnedGameObjects = {}

Game:GameObjectDeleted(function(OBJ)
	SpawnedGameObjects[OBJ] = nil
end)

function ClearAllSpawnedGameObjects()
	PrintFast("go")
	PrintFast(Table:Pairs({})) -- Калово доделай!!!!
	local x,y,z = Table:Pairs({})
	PrintFast(x.." "..y.." "..z)
	for i, Key, Value in Table:Pairs(SpawnedGameObjects) do
		PrintFast("["..i.."] "..Key.." = "..Value)
	end
end

function SelectSpawnGameObject(Path)
	SelectedSpawningGameObject = Path
	GameObject:SetText(Text_SelectedGameObject, Path)
end

Controls:KeyPressed(function(Key)
	if (UIOpened) then return end

	if (Key == KEY_E) then
		SpawnGameObject(SelectedSpawningGameObject,true,MouseWorldPosition())
	end
	if (Key == KEY_Q) then
		for a = 0, 100 do
			SpawnGameObject(SelectedSpawningGameObject,false,MouseWorldPosition())
		end
	end
	
	if (Key == KEY_C) then
		ClearAllSpawnedGameObjects()
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
		SpawnedGameObjects[OBJ] = Path
	end
end