#include "../rootlib.h"

#include "rootcomponentbase.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Root::RootComponentBase)
METATABLE_END(Engine::Root::RootComponentBase)

namespace Engine {
namespace Root {
    RootComponentBase::RootComponentBase(Root &root)
        : mRoot(root)
    {
    }

}
}
