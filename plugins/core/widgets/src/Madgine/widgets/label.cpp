#include "../widgetslib.h"

#include "label.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Madgine/fontloader/fontloader.h"

METATABLE_BEGIN_BASE(Engine::Widgets::Label, Engine::Widgets::WidgetBase)
MEMBER(mText)
MEMBER(mFontSize)
MEMBER(mPivot)
PROPERTY(Font, getFont, setFont)
METATABLE_END(Engine::Widgets::Label)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Label, Engine::Widgets::WidgetBase)
FIELD(mFontSize)
FIELD(mText)
FIELD(mPivot)
ENCAPSULATED_FIELD(mFont, getFontName, setFontName)
SERIALIZETABLE_END(Engine::Widgets::Label)

namespace Engine {
namespace Widgets {

    WidgetClass Label::getClass() const
    {
        return WidgetClass::LABEL;
    }

    std::vector<std::pair<std::vector<Vertex>, TextureSettings>> Label::vertices(const Vector3 &screenSize)
    {
        Vector3 pos = (getEffectivePosition() * screenSize) / screenSize;
        Vector3 size = (getEffectiveSize() * screenSize) / screenSize;
        pos.z = depth();

        if (mFont && mFont.available()) {
            //mFont->setPersistent(true);
            return { renderText(mText, pos, size.xy(), mFont, size.z * mFontSize, mPivot, screenSize) };
        }

        return {};
    }

    std::string_view Label::getFontName() const
    {
        return mFont.name();
    }

    void Label::setFontName(std::string_view name)
    {
        mFont.load(name);
    }

    Render::FontLoader::Resource *Label::getFont() const
    {
        return mFont.resource();
    }

    void Label::setFont(Render::FontLoader::Resource *font)
    {
        mFont = font;
    }

}
}
