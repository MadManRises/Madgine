#include "../widgetslib.h"

#include "button.h"

#include "Modules/math/vector4.h"

#include "vertex.h"

#include <iostream>

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "fontloader.h"

#include "imageloader.h"

METATABLE_BEGIN_BASE(Engine::Widgets::Button, Engine::Widgets::WidgetBase)
MEMBER(mText)
MEMBER(mFontSize)
PROPERTY(Font, getFont, setFont)
PROPERTY(Image, image, setImage)
METATABLE_END(Engine::Widgets::Button)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Button, Engine::Widgets::WidgetBase)
FIELD(mText)
FIELD(mFontSize)
ENCAPSULATED_FIELD(mFont, getFontName, setFontName)
ENCAPSULATED_FIELD(Image, getImageName, setImageByName)
SERIALIZETABLE_END(Engine::Widgets::Button)

namespace Engine {
namespace Widgets {

        void Button::setImageByName(const std::string &name)
    {
        setImage(Resources::ImageLoader::getSingleton().get(name));
    }

    void Button::setImage(Resources::ImageLoader::ResourceType *image)
    {
        mImage = image;
    }

    std::string Button::getImageName() const
    {
        return mImage ? mImage->name() : "";
    }

    Resources::ImageLoader::ResourceType *Button::image() const
    {
        return mImage;
    }

    Resources::ImageLoader::ResourceType *Button::resource() const
    {
        return mImage;
    }

    Threading::SignalStub<> &Button::clickEvent()
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
        result.push_back({ v, color, { 0.0f, 0.0f } });
        v.x += size.x;
        result.push_back({ v, color, { 1.0f, 0.0f } });
        v.y += size.y;
        result.push_back({ v, color, { 1.0f, 1.0f } });
        result.push_back({ v, color, { 1.0f, 1.0f } });
        v.x -= size.x;
        result.push_back({ v, color, { 0.0f, 1.0f } });
        v.y -= size.y;
        result.push_back({ v, color, { 0.0f, 0.0f } });

        returnSet.push_back({ result, {} });

        if (mFont) {
            //mFont->setPersistent(true);
            std::pair<std::vector<Vertex>, Render::TextureDescriptor> fontVertices = renderText(mText, pos + 0.5f * size, mFont, mFontSize, { 0.5f, 0.5f }, screenSize);
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
        return mFont.name();
    }

    void Button::setFontName(const std::string &name)
    {
        mFont.load(name);
    }

    Render::FontLoader::ResourceType *Button::getFont() const
    {
        return mFont.resource();
    }

    void Button::setFont(Render::FontLoader::ResourceType *font)
    {
        mFont = font;
    }

}
}