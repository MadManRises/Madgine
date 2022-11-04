#include "../widgetslib.h"

#include "label.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

METATABLE_BEGIN_BASE(Engine::Widgets::Label, Engine::Widgets::WidgetBase)
MEMBER(mText)
NAMED_MEMBER(TextData, mTextRenderData)
METATABLE_END(Engine::Widgets::Label)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Label, Engine::Widgets::WidgetBase)
FIELD(mText)
FIELD(mTextRenderData)
SERIALIZETABLE_END(Engine::Widgets::Label)

namespace Engine {
namespace Widgets {

    WidgetClass Label::getClass() const
    {
        return WidgetClass::LABEL;
    }

    std::vector<std::pair<std::vector<Vertex>, TextureSettings>> Label::vertices(const Vector3 &screenSize, size_t layer)
    {
        if (!mTextRenderData.available())
            return {};

        Vector3 pos { getAbsolutePosition() / screenSize.xy(), static_cast<float>(depth(layer)) };
        Vector3 size = getAbsoluteSize() / screenSize;

        return { mTextRenderData.render(mText, pos, size, screenSize.xy()) };
    }

}
}
