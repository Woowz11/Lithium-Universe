function CreateTestObject()
	-- Создать игровой объект
	local Obj = GameObject:Create("Test omagad")
	
	-- Указать, что этот объект будет использован для спавна (делает его не интерактивным), в строке указывается айди объекта, для того что-бы его можно было использовать как родитель в других модах или у себя
	Resources:SaveGameObject(Obj, "TestObject");
	
	-- Добавить объект в меню спавна
	Creative:AddSpawning({
		-- Название объекта в меню спавна
		["Name"] = "Test GameObject",
		-- Краткое описание объекта
		["Description"] = "OMAAGAAD",
		-- Ивент вызываемый при спавне этого объекта
		["SpawningEvent"] = function(Position, Flipped)
			-- Спавн объекта
			local NewObj = Resources:CloneGameObject("Vanilla:TestObject")
			GameObject:SetPosition(NewObj,Position)
			-- Вернуть объект который будет добавлен в список созданных (для удаления)
			return NewObj
		end,
		-- Иконка объекта
		["Icon"] = nil,
		-- Более раскрытое описание
		["ExtraDescription"] = "",
		-- Категория объекта
		["Category"] = "Test"
	})
end

function LoadVanillaGameObject()
	CreateTestObject()
end