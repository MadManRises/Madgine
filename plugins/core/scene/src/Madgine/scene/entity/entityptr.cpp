#include "../../scenelib.h"

#include "entity.h"
#include "entityptr.h"

#include "../scenemanager.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/streams/operations.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

METATABLE_BEGIN(Engine::Scene::Entity::EntityPtr)
PROXY(get)
METATABLE_END(Engine::Scene::Entity::EntityPtr)

namespace Engine {
namespace Scene {
    namespace Entity {

        static constexpr uintptr_t mask = 3 ^ static_cast<uintptr_t>(Serialize::UnitIdTag::SYNCABLE);

        EntityPtr::EntityPtr(const EntityPtr &other)
            : mEntity(other.mEntity)
        {
            if (holdsRef())
                getBlock()->incRef();
        }

        EntityPtr::~EntityPtr()
        {
            reset();
        }

        EntityPtr::EntityPtr(ControlBlock<Entity> &entity)
            : mEntity(reinterpret_cast<uintptr_t>(&entity) | mask)
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
                mEntity &= ~mask;
            }
        }

        void EntityPtr::reset()
        {
            if (holdsRef())
                getBlock()->decRef();
            mEntity = 0;
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

        bool EntityPtr::isDead() const
        {
            return !holdsRef() || getBlock()->dead();
        }

        void EntityPtr::readState(Serialize::SerializeInStream &in, const char *name)
        {
            Entity *ptr;
            Serialize::read(in, ptr, name);
            mEntity = reinterpret_cast<uintptr_t>(ptr);
        }

        void EntityPtr::writeState(Serialize::SerializeOutStream &out, const char *name) const
        {
            Serialize::write(out, get(), name);
        }

        void EntityPtr::applySerializableMap(Serialize::SerializeInStream &in)
        {            
            if (mEntity & static_cast<uintptr_t>(Serialize::UnitIdTag::SYNCABLE)) {
                Entity *ptr = reinterpret_cast<Entity *>(mEntity);
                Serialize::UnitHelper<Entity *>::applyMap(in, ptr);
                *this = *ControlBlock<Entity>::fromPtr(ptr); //TODO
            }
        }

        bool EntityPtr::holdsRef() const
        {
            return mEntity & mask;
        }

        ControlBlock<Entity> *EntityPtr::getBlock() const
        {
            return reinterpret_cast<ControlBlock<Entity> *>(mEntity & ~mask);
        }
    }
}
}