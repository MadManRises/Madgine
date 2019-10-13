#include "../../clientlib.h"

#include "button.h"

#include "Modules/math/vector4.h"

#include "../vertex.h"

#include <iostream>

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Modules/resources/resource.h"

#include "Modules/font/font.h"

METATABLE_BEGIN(Engine::GUI::Button)
MEMBER(mText)
MEMBER(mFontSize)
MEMBER(mFont)
METATABLE_END(Engine::GUI::Button)

SERIALIZETABLE_INHERIT_BEGIN(Engine::GUI::Button, Engine::GUI::WidgetBase)
FIELD(mFontSize)
FIELD(mText)
ENCAPSULATED_FIELD(mFont, getFontName, setFontName)
SERIALIZETABLE_END(Engine::GUI::Button)

namespace Engine {
namespace GUI {

    SignalSlot::SignalStub<> &Button::clickEvent()
    {
        return mClicked;
    }

 

    std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> Button::vertices(const Vector3 &screenSize)
    {

        std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> returnSet;
        std::vector<Vertex> result;

        Vector3 pos = (getAbsolutePosition() * screenSize) / screenSize;
        Vector3 size = (getAbsoluteSize() * screenSize) / screenSize;
        pos.z = depth();

        Vector4 color = mHovered ? Vector4 { 1.0f, 0.1f, 0.1f, 1.0f } : Vector4 { 0.4f, 0.4f, 0.4f, 1.0f };

        Vector3 v = pos;        
        result.push_back({ v, color });
        v.x += size.x;
        result.push_back({ v, color });
        v.y += size.y;
        result.push_back({ v, color });
        result.push_back({ v, color });
        v.x -= size.x;
        result.push_back({ v, color });
        v.y -= size.y;
        result.push_back({ v, color });

        returnSet.push_back({ result, {} });

        if (mFont) {
            mFont->setPersistent(true);
            if (mFont->load()) {
                std::pair<std::vector<Vertex>, Render::TextureDescriptor> fontVertices = renderText(mText, pos + 0.5f * size, mFont->data().get(), mFontSize, { 0.5f, 0.5f }, screenSize);
                if (!fontVertices.first.empty())
                    returnSet.push_back(fontVertices);
            }
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
        mClicking = false;
        return true;
    }

    bool Button::injectPointerPress(const Input::PointerEventArgs &arg)
    {
        mClicking = true;
        return true;
    }

    bool Button::injectPointerRelease(const Input::PointerEventArgs &arg)
    {
        if (mClicking)
            emitClicked();
        return true;
    }

    void Button::emitClicked()
    {
        mClicked.emit();
    }

    WidgetClass Button::getClass() const
    {
        return WidgetClass::BUTTON_CLASS;
    }

    std::string Button::getFontName() const
    {
        return mFont ? mFont->name() : "";
    }

    void Button::setFontName(const std::string &name)
    {
        mFont = Font::FontLoader::getSingleton().get(name);
    }
}
}