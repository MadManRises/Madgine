#include "../firstpartylib.h"

#include "firstpartyservices.h"

#include "Madgine/root/root.h"

#include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::FirstParty::FirstPartyServices)
METATABLE_END(Engine::FirstParty::FirstPartyServices)

VIRTUALUNIQUECOMPONENTBASE(Engine::FirstParty::FirstPartyServices)

namespace Engine {
namespace FirstParty {

    FirstPartyServices::FirstPartyServices(Root::Root &root)
        : VirtualRootComponentBase(root)
    {
    }

}
}
