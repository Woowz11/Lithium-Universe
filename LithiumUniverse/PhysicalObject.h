#include <string>
#include <GLM/glm.hpp>
#include "RenderedObject.h";

/* Объект с физикой */
class PhysicalObject : public RenderedObject {
public:

	bool Static                  = false;            /* Объект статичный?      */
	glm::vec2 GravityModificator = glm::vec2(1, 1);  /* Модификатор гравитации */
	float Friction               = 1;                /* Трение                 */
	float Elasticity             = 1;                /* Упругость              */
	float Mass                   = 1;                /* Масса                  */

	PhysicalObject(std::string Name_) : RenderedObject(Name_, RO_UI) {}

	/* Сделать статичным */
	void MakeStatic() {
		Static = true;
	}

	/* Сделать физичным */
	void MakePhysical() {
		Static = false;
	}
};