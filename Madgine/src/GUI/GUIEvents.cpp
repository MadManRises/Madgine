#include "madginelib.h"
#include "GUIEvents.h"

std::ostream &operator << (std::ostream &str, const Engine::GUI::MouseEventArgs &me) {
	str << "{ Button: " << me.button << std::endl;
	str << "  MoveDelta: " << me.moveDelta << std::endl;
	str << "  Position: " << me.position << std::endl;
	str << "  ScrollWheel: " << me.scrollWheel << " }" << std::endl;
	return str;
}