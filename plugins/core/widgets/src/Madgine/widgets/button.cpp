#include "../widgetslib.h"

#include "button.h"

#include "Meta/math/vector4.h"

#include "vertex.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "fontloader.h"

#include "imageloader.h"

METATABLE_BEGIN_BASE(Engine::Widgets::Button, Engine::Widgets::WidgetBase)
MEMBER(mText)
MEMBER(mFontSize)
MEMBER(mPivot)
PROPERTY(Font, font, setFont)
PROPERTY(Image, image, setImage)
METATABLE_END(Engine::Widgets::Button)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Button, Engine::Widgets::WidgetBase)
FIELD(mText)
FIELD(mFontSize)
FIELD(mPivot)
ENCAPSULATED_FIELD(mFont, fontName, setFontName)
ENCAPSULATED_FIELD(Image, imageName, setImageName)
SERIALIZETABLE_END(Engine::Widgets::Button)

namespace Engine {
namespace Widgets {

    void Button::setImageName(std::string_view name)
    {
        setImage(Resources::ImageLoader::getSingleton().get(name));
    }

    void Button::setImage(Resources::ImageLoader::ResourceType *image)
    {
        mImage = image;
    }

    std::string_view Button::imageName() const
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

    std::vector<std::pair<std::vector<Vertex>, TextureSettings>> Button::vertices(const Vector3 &screenSize)
    {

        std::vector<std::pair<std::vector<Vertex>, TextureSettings>> returnSet;
        std::vector<Vertex> result;

        Vector3 pos = (getEffectivePosition() * screenSize) / screenSize;
        Vector3 size = (getEffectiveSize() * screenSize) / screenSize;
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

        if (mFont.available() /*&& mFont->mTexture.available()*/) {
            //mFont->setPersistent(true);
            std::pair<std::vector<Vertex>, TextureSettings> fontVertices = renderText(mText, pos, size.xy(), mFont, size.z * mFontSize, mPivot, screenSize);
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
        return WidgetClass::BUTTON;
    }

    std::string_view Button::fontName() const
    {
        return mFont.name();
    }

    void Button::setFontName(std::string_view name)
    {
        mFont.load(name);
    }

    Render::FontLoader::ResourceType *Button::font() const
    {
        return mFont.resource();
    }

    void Button::setFont(Render::FontLoader::HandleType font)
    {
        mFont = std::move(font);
    }

}
}