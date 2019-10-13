#include "../../clientlib.h"

#include "label.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"
#include "Modules/serialize/serializetable_impl.h"

#include "../vertex.h"

#include "Modules/font/font.h"

METATABLE_BEGIN(Engine::GUI::Label)
MEMBER(mText)
MEMBER(mFontSize)
MEMBER(mFont)
METATABLE_END(Engine::GUI::Label)

SERIALIZETABLE_INHERIT_BEGIN(Engine::GUI::Label, Engine::GUI::WidgetBase)
FIELD(mFontSize)
FIELD(mText)
ENCAPSULATED_FIELD(mFont, getFontName, setFontName)
SERIALIZETABLE_END(Engine::GUI::Label)

RegisterType(Engine::GUI::Label);

namespace Engine {
namespace GUI {

    WidgetClass Label::getClass() const
    {
        return WidgetClass::LABEL_CLASS;
    }

    std::vector<std::pair<std::vector<Vertex>, Render::TextureDescriptor>> Label::vertices(const Vector3 &screenSize)
    {
        Vector3 pos = (getAbsolutePosition() * screenSize) / screenSize;
        pos.z = depth();
        
		if (mFont) {
            mFont->setPersistent(true);
            if (mFont->load()) {
                return { renderText(mText, pos, mFont->data().get(), mFontSize, { 0, 0 }, screenSize) };        
            }
        }

		return {};
    }

    std::string Label::getFontName() const
    {
        return mFont ? mFont->name() : "";
    }

    void Label::setFontName(const std::string &name)
    {
        mFont = Font::FontLoader::getSingleton().get(name);
    }

}
}
