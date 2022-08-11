#include "../widgetslib.h"

#include "image.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/imageloader/imageloader.h"

#include "widgetmanager.h"


METATABLE_BEGIN_BASE(Engine::Widgets::Image, Engine::Widgets::WidgetBase)
NAMED_MEMBER(Image, mImageRenderData)
METATABLE_END(Engine::Widgets::Image)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Image, Engine::Widgets::WidgetBase)
FIELD(mImageRenderData)
SERIALIZETABLE_END(Engine::Widgets::Image)

namespace Engine {
namespace Widgets {

    std::vector<std::pair<std::vector<Vertex>, TextureSettings>> Image::vertices(const Vector3 &screenSize)
    {
        const Atlas2::Entry* entry = manager().lookUpImage(mImageRenderData.image());
        if (!entry)
            return {};

        Vector3 pos = (getEffectivePosition() * screenSize) / screenSize;
        Vector3 size = (getEffectiveSize() * screenSize) / screenSize;
        pos.z = depth();

        return { { mImageRenderData.renderImage(pos, size.xy(), screenSize.xy(), *entry), { 0 } } };
    }

    WidgetClass Image::getClass() const
    {
        return WidgetClass::IMAGE;
    }

}
}
