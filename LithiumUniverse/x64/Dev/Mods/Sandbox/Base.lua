Resources:LoadScript("Sandbox:Controls.lua")
Resources:LoadScript("Sandbox:GUI.lua")

Game:UILoading(function()
	CreateGUI()
	SelectSpawnGameObject("Vanilla:1Mx1M")
end)

Print("Sandbox loaded!")