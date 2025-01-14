#pragma once

/* Тип коллайдера */
enum CLDR_Type {
	CLDR_None,
	CLDR_Square,
	CLDR_Circle,
	CLDR_Triangle,
	CLDR_Point,
	CLDR_Line,
	CLDR_Custom
};

class Collider {
public:
	CLDR_Type Type;
	Collider(CLDR_Type type){
		Type = type;
	}
};