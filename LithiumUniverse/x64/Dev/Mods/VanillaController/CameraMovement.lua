Controls:KeyPressed(KEY_HOME, function() 
	Camera:Reset()
end)

function UpdateCamera()
	local Shift = Controls:KeyIsPressed(KEY_LEFT_SHIFT)
	local Control = Controls:KeyIsPressed(KEY_LEFT_CONTROL)

	local Speed = IfThen(Shift,3,IfThen(Control,0.3,1))

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
end