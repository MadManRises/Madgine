#include "../../scenelib.h"

#include "entity.h"
#include "entityptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        EntityPtr::EntityPtr(const typename GenerationVector<Entity>::iterator it)
            : mIt(it)
        {
        }

        Entity *EntityPtr::operator->() const
        {
            return &*mIt;
        }

        EntityPtr::operator bool() const
        {
            return mIt.index();
        }

        EntityPtr::operator Entity *() const
        {
            return &*mIt;
        }

        bool EntityPtr::operator==(const EntityPtr &other) const
        {
            return mIt == other.mIt;
        }

    }
}
}