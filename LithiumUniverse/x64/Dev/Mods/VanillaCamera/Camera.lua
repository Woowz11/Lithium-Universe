function GetSpeed()
	local Shift = Controls:KeyIsPressed(KEY_LEFT_SHIFT)
	local Control = Controls:KeyIsPressed(KEY_LEFT_CONTROL)
	
	return IfThen(Shift,3,IfThen(Control,0.3,1))
end

Controls:KeyPressedSingle(KEY_HOME, function() 
	Camera:Reset()
end)

Controls:MouseScroll(function(ScrollDir)
	Camera:MoveZoomCustom(ScrollDir * GetSpeed() * 0.25, 1)
	
	--local Dif = MouseWorldPosition() - Camera:Position()
	--Camera:Move(Dif * ScrollDir * 15 / Camera:Zoom())
end)

local CameraDrag = false
local CameraDragOffset = nil
local CameraDragPosition = nil
Controls:MousePressed(function(Key)
	if Key == KEY_MOUSE_MIDDLE then
		CameraDragOffset = MouseLocalPosition()
		CameraDragPosition = Camera:Position()
		CameraDrag = true
	end
end)

Controls:MouseReleased(function(Key)
	if Key == KEY_MOUSE_MIDDLE then
		CameraDrag = false
	end
end)

Game:Update(function()
	local Speed = GetSpeed()

	-- Движение камеры
	local W = Controls:KeyIsPressed(KEY_W)
	local S = Controls:KeyIsPressed(KEY_S)
	local D = Controls:KeyIsPressed(KEY_D)
	local A = Controls:KeyIsPressed(KEY_A)
	
	local DirY = 0
	if (W and not S) then
		DirY = 1
	end
	if (S and not W) then
		DirY = -1
	end
	
	local DirX = 0
	if (D and not A) then
		DirX = 1
	end
	if (A and not D) then
		DirX = -1
	end
	
	local CameraMovingDirection = Vector2(DirX, DirY) * Speed
	Camera:Move(CameraMovingDirection)
	
	if (CameraDrag) then
		local Dif = CameraDragOffset - MouseLocalPosition()
		Dif = Dif * Camera:Zoom() * ScreenScale
		Camera:SetPosition(CameraDragPosition + Dif:Rotate(-Camera:Orientation()))
	end
	
	-- Зум камеры
	local Plus = Controls:KeyIsPressed(KEY_EQUAL)
	local Minus = Controls:KeyIsPressed(KEY_MINUS)
	
	if (Plus and not Minus) then
		Camera:MoveZoom(Speed)
	end
	if (Minus and not Plus) then
		Camera:MoveZoom(-Speed)
	end
	
	-- Вращение камеры
	local Right = Controls:KeyIsPressed(KEY_RIGHT)
	local Left = Controls:KeyIsPressed(KEY_LEFT)
	
	if (Right and not Left) then
		Camera:Rotate(Speed)
	end
	if (Left and not Right) then
		Camera:Rotate(-Speed)
	end
end)