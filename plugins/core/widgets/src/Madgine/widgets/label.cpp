#include "../widgetslib.h"

#include "label.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"


#include "fontloader.h"


METATABLE_BEGIN_BASE(Engine::Widgets::Label, Engine::Widgets::WidgetBase)
MEMBER(mText)
MEMBER(mFontSize)
PROPERTY(Font, getFont, setFont)
METATABLE_END(Engine::Widgets::Label)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Label, Engine::Widgets::WidgetBase)
FIELD(mFontSize)
FIELD(mText)
ENCAPSULATED_FIELD(mFont, getFontName, setFontName)
SERIALIZETABLE_END(Engine::Widgets::Label)



namespace Engine {
namespace Widgets {

    WidgetClass Label::getClass() const
    {
        return WidgetClass::LABEL_CLASS;
    }

    std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> Label::vertices(const Vector3 &screenSize)
    {
        Vector3 pos = (getAbsolutePosition() * screenSize) / screenSize;
        pos.z = depth();

        if (mFont) {
            //mFont->setPersistent(true);
            return { renderText(mText, pos, mFont, mFontSize, { 0, 0 }, screenSize) };
        }

        return {};
    }

    std::string Label::getFontName() const
    {
        return mFont.name();
    }

    void Label::setFontName(const std::string &name)
    {
        mFont.load(name);
    }
	
    Render::FontLoader::ResourceType *Label::getFont() const
    {
        return mFont.resource();
    }

    void Label::setFont(Render::FontLoader::ResourceType *font)
    {
        mFont = font;
    }

}
}
