local SpawningGameObjects = {}

Storage:SaveFunction("Sandbox:AddToSpawn",function(V)
	Table:Add(SpawningGameObjects,V)
end)

--[[Storage:SaveFunction("Sandbox:Spawn",function(V)
	local ID = V[1]
	local SpawnExtraInfo = V[2]
	local Position = SpawnExtraInfo[1]
	Print(TypeOf(Position))
	Print(MouseWorldPosition())
	Print(TypeOf(MouseWorldPosition()))
	local Flipped  = SpawnExtraInfo[2]
	
	local SpawningInfo = SpawningGameObjects[ID]
	Print(Table:ToString({ID,SpawnExtraInfo,Position,Vector2(2,3),SpawningInfo}))
	SpawningInfo["SpawningEvent"](Position, Flipped)
end)]]

Storage:SaveFunction("Test",function(V)
	Print(Table:ToString(Deserialize({{["__type"] = "v2", ["x"] = 2, ["y"] = 1}})))
	Print(V)
	Table:Pairs(V[1],function(i,k,v)
		Print("("..i..") "..k.." = "..v)
	end)
	Print("popo")
	Print("ALO BLAT -> "..V[1]["__type"])
	Print("popo2")
	Print(Table:ToString(Deserialize(V[1])))
end)