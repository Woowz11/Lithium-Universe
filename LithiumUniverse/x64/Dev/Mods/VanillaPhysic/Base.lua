Game:UpdateEveryGameObject(function(Scene)
	for i = 1, #Scene do
		local Obj = Scene[i]
		
		if (GameObject:GetCreatedFromPlayer(Obj)==true) then
			if (GameObject:GetPosition(Obj).Y < -300) then
				GameObject:Delete(Obj)
			end
		end
	end
end)