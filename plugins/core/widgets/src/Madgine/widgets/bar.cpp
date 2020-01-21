#include "../widgetslib.h"

#include "bar.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

METATABLE_BEGIN(Engine::Widgets::Bar)
METATABLE_END(Engine::Widgets::Bar)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Widgets::Bar, Engine::Widgets::WidgetBase)
SERIALIZETABLE_END(Engine::Widgets::Bar)

namespace Engine {
namespace Widgets {
    void Bar::setRatio(float f)
    {
    }
    WidgetClass Bar::getClass() const
    {
        return WidgetClass::BAR_CLASS;
    }
}
}