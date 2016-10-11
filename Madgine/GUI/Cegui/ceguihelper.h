#ifndef GAME_CEGUI_CEGUIHELPER_H
#define GAME_CEGUI_CEGUIHELPER_H

#include "CEGUI\CEGUI.h"

namespace Engine {
namespace Cegui {

namespace CEGUIHelper {

CEGUI::Vector2f screenToWindowRelative(const CEGUI::Vector2f &v, CEGUI::Window *w);
CEGUI::Vector2f absToWindowRelative(const CEGUI::Vector2f &v, CEGUI::Window *w);
float squaredVectorLength(const CEGUI::Vector2f &v);
//CEGUI::MouseButton convertButton(OIS::MouseButtonID buttonID);

}

} // namespace Cegui
}

#endif // GAME_CEGUI_CEGUIHELPER_H
