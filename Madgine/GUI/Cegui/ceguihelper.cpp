#include "libinclude.h"
#include "ceguihelper.h"

namespace Engine {
namespace Cegui {

namespace CEGUIHelper {

CEGUI::Vector2f screenToWindowRelative(const CEGUI::Vector2f &v, CEGUI::Window *w)
{
    CEGUI::Vector2f windowPos = CEGUI::CoordConverter::screenToWindow(*w, v);

    return absToWindowRelative(windowPos, w);
}

CEGUI::Vector2f absToWindowRelative(const CEGUI::Vector2f &v, CEGUI::Window *w)
{
    return CEGUI::Vector2f(
                v.d_x / w->getPixelSize().d_width,
                v.d_y / w->getPixelSize().d_height
                );
}

float squaredVectorLength(const CEGUI::Vector2f &v)
{
    return v.d_x * v.d_x + v.d_y * v.d_y;
}




}

}
} // namespace Cegui

