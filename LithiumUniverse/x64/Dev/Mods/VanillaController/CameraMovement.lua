function MoveCamera(xVel, yVel)
	Print("x: "..xVel.." | y: "..yVel)
end

Controls:KeyPress(KEY_W,function() MoveCamera( 0, 1) end)
Controls:KeyPress(KEY_S,function() MoveCamera( 0,-1) end)
Controls:KeyPress(KEY_D,function() MoveCamera( 1, 0) end)
Controls:KeyPress(KEY_A,function() MoveCamera(-1, 0) end)