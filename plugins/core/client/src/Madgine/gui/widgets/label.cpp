#include "../../clientlib.h"

#include "label.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"
#include "Modules/serialize/serializetable_impl.h"

#include "../vertex.h"

#include "fontloader.h"

METATABLE_BEGIN(Engine::Widgets::Label)
MEMBER(mText)
MEMBER(mFontSize)
MEMBER(mFont)
METATABLE_END(Engine::Widgets::Label)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Label, Engine::Widgets::WidgetBase)
FIELD(mFontSize)
FIELD(mText)
ENCAPSULATED_FIELD(mFont, getFontName, setFontName)
SERIALIZETABLE_END(Engine::Widgets::Label)

RegisterType(Engine::Widgets::Label);

namespace Engine {
namespace Widgets {

    WidgetClass Label::getClass() const
    {
        return WidgetClass::LABEL_CLASS;
    }

    std::vector<std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor>> Label::vertices(const Vector3 &screenSize)
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
