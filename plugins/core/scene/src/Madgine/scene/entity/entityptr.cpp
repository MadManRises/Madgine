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
            if (holdsRef())
                getBlock()->decRef();
        }

        EntityPtr::~EntityPtr()
        {
            if (holdsRef())
                getBlock()->decRef();
        }

        EntityPtr::EntityPtr(ControlBlock<Entity> &entity)
            : mEntity(reinterpret_cast<uintptr_t>(&entity) | 1)
        {
            entity.incRef();
        }

        EntityPtr &EntityPtr::operator=(const EntityPtr &other)
        {
            if (holdsRef())
                getBlock()->decRef();
            mEntity = other.mEntity;
            if (holdsRef())
                getBlock()->incRef();
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

        void EntityPtr::update() const
        {
            if (holdsRef() && getBlock()->dead()) {
                getBlock()->decRef();
                mEntity &= ~1;
            }
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
            return getBlock() ? getBlock()->get() : nullptr;
        }

        bool EntityPtr::operator==(const EntityPtr &other) const
        {
            update();
            other.update();
            return mEntity == other.mEntity;
        }

        bool EntityPtr::operator<(const EntityPtr &other) const
        {
            update();
            other.update();
            return mEntity < other.mEntity;
        }

        TypedScopePtr EntityPtr::customScopePtr() const
        {
            return get();
        }

        bool EntityPtr::isDead() const
        {
            return !holdsRef() || getBlock()->dead();
        }

        bool EntityPtr::holdsRef() const
        {
            return mEntity & 1;
        }

        ControlBlock<Entity> *EntityPtr::getBlock() const
        {
            return reinterpret_cast<ControlBlock<Entity> *>(mEntity & ~1);
        }
    }
}
}