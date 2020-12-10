#include "../../scenelib.h"

#include "entity.h"
#include "entityptr.h"

#include "../scenemanager.h"

#include "entitycontrolblock.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        EntityPtr::EntityPtr(const EntityPtr &other)
            : mEntity(other.mEntity)
        {
            if (mEntity)
                mEntity->incRef();
        }

        EntityPtr::~EntityPtr()
        {
            if (mEntity)
                mEntity->decRef();
        }

        EntityPtr::EntityPtr(ControlBlock<Entity> &entity)
            : mEntity(&entity)
        {
            if (mEntity)
                mEntity->incRef();
        }

        EntityPtr &EntityPtr::operator=(const EntityPtr &other)
        {
            if (mEntity)
                mEntity->decRef();
            mEntity = other.mEntity;
            if (mEntity)
                mEntity->incRef();
            return *this;
        }

        EntityPtr &EntityPtr::operator=(EntityPtr &&other)
        {
            std::swap(mEntity, other.mEntity);
            return *this;
        }

        Entity *EntityPtr::operator->() const
        {
            return get();
        }

        void EntityPtr::update() const {

        }

        /*EntityPtr::operator bool() const
        {            
            return get();
        }*/

        EntityPtr::operator Entity *() const
        {            
            return get();
        }

        Entity *EntityPtr::get() const
        {
            update();
            return mEntity ? mEntity->get() : nullptr;            
        }

        bool EntityPtr::operator==(const EntityPtr &other) const
        {
            update();
            other.update();
            return mEntity == other.mEntity;
        }

        TypedScopePtr EntityPtr::customScopePtr() const
        {
            return mEntity->get();
        }

        bool EntityPtr::isDead() const
        {
            return mEntity->dead();
        }

    }
}
}