SelectedSpawningGameObject = "Vanilla:1Mx1M"

SlowingTime = 0.1

GameSlowing = false
GamePaused = false

Controls:KeyPressed(function(Key)
	if (Key == KEY_E) then
		SpawnGameObject(SelectedSpawningGameObject,true,MouseWorldPosition())
	end
	if (Key == KEY_Q) then
		for a = 0, 100 do
			SpawnGameObject(SelectedSpawningGameObject,false,MouseWorldPosition())
		end
	end
	
	if (Key == KEY_1) then
		SelectedSpawningGameObject = "Vanilla:1Mx1M"
	end
	if (Key == KEY_2) then
		SelectedSpawningGameObject = "Vanilla:HumanSize"
	end
	if (Key == KEY_3) then
		SelectedSpawningGameObject = "Vanilla:Box"
	end
	if (Key == KEY_4) then
		SelectedSpawningGameObject = ""
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
		GameObject:SetData(OBJ,88612412,true)
	end
end