#include "../widgetslib.h"

#include "button.h"

#include "widgetmanager.h"

#include "Meta/math/vector4.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/fontloader/fontloader.h"

#include "Madgine/imageloader/imageloader.h"

METATABLE_BEGIN_BASE(Engine::Widgets::Button, Engine::Widgets::WidgetBase)
MEMBER(mText)
NAMED_MEMBER(TextData, mTextRenderData)
NAMED_MEMBER(Image, mImageRenderData)
METATABLE_END(Engine::Widgets::Button)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Button, Engine::Widgets::WidgetBase)
FIELD(mText)
FIELD(mTextRenderData)
FIELD(mImageRenderData)
SERIALIZETABLE_END(Engine::Widgets::Button)

namespace Engine {
namespace Widgets {

    Threading::SignalStub<> &Button::clickEvent()
    {
        return mClicked;
    }

    std::vector<std::pair<std::vector<Vertex>, TextureSettings>> Button::vertices(const Vector3 &screenSize, size_t layer)
    {
        const Atlas2::Entry *entry = manager().lookUpImage(mImageRenderData.image());
        if (!entry)
            return {};

        Vector3 pos = (getEffectivePosition() * screenSize) / screenSize;
        Vector3 size = (getEffectiveSize() * screenSize) / screenSize;
        pos.z = depth(layer);

        std::vector<std::pair<std::vector<Vertex>, TextureSettings>> returnSet;

        Vector4 color = mHovered ? Vector4 { 1.0f, 0.1f, 0.1f, 1.0f } : Vector4 { 0.4f, 0.4f, 0.4f, 1.0f };

        returnSet.push_back({ mImageRenderData.renderImage(pos, size.xy(), screenSize.xy(), *entry, color), { 0 } });

        if (mTextRenderData.available()) {
            std::pair<std::vector<Vertex>, TextureSettings> fontVertices = mTextRenderData.render(mText, pos, size, screenSize.xy());
            if (!fontVertices.first.empty())
                returnSet.push_back(fontVertices);
        }

        return returnSet;
    }

    bool Button::injectPointerEnter(const Input::PointerEventArgs &arg)
    {
        mHovered = true;
        return true;
    }

    bool Button::injectPointerLeave(const Input::PointerEventArgs &arg)
    {
        mHovered = false;
        return true;
    }

    bool Button::injectPointerClick(const Input::PointerEventArgs &arg)
    {
        emitClicked();
        return true;
    }

    void Button::emitClicked()
    {
        mClicked.emit();
    }

    WidgetClass Button::getClass() const
    {
        return WidgetClass::BUTTON;
    }

}
}