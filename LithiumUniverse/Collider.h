#pragma once

/* Тип коллайдера */
enum CLDR_Type {
	CLDR_None,
	CLDR_Fill,
	CLDR_Circle,
	CLDR_Custom
};

class Collider {
public:
	CLDR_Type Type;
	Collider(CLDR_Type type){
		Type = type;
	}
};