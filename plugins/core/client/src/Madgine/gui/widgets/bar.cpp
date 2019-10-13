#include "../../clientlib.h"

#include "bar.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::GUI::Bar)
METATABLE_END(Engine::GUI::Bar)

SERIALIZETABLE_INHERIT_BEGIN(Engine::GUI::Bar, Engine::GUI::WidgetBase)
SERIALIZETABLE_END(Engine::GUI::Bar)

namespace Engine {
namespace GUI {
    void Bar::setRatio(float f)
    {
    }
    WidgetClass Bar::getClass() const
    {
        return WidgetClass::BAR_CLASS;
    }
}
}