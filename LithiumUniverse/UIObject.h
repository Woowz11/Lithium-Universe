#include <string>
#include "RenderedObject.h";

/* Объект интерфейса */
class UIObject : public RenderedObject {
public:
	UIObject(std::string Name_) : RenderedObject(Name_, RO_UI) {}
};