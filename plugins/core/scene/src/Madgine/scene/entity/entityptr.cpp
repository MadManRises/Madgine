#include "../../scenelib.h"

#include "entity.h"
#include "entityptr.h"

#include "../scenemanager.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/serialize/streams/operations.h"

METATABLE_BEGIN(Engine::Scene::Entity::EntityPtr)
PROXY(get)
METATABLE_END(Engine::Scene::Entity::EntityPtr)

namespace Engine {
namespace Scene {
    namespace Entity {

        struct EntityPtr::ControlBlockDummy {

            SceneManager::ControlBlock *operator->() const
            {
                return mBlock;
            }

             operator SceneManager::ControlBlock *() const
            {
                return mBlock;
            }

            SceneManager::ControlBlock *mBlock;
        };

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

        EntityPtr::EntityPtr(Entity *entity)
            : mEntity(reinterpret_cast<uintptr_t>(SceneManager::ControlBlock::fromPtr(entity)))
        {
            if (mEntity) {
                assert(!getBlock()->dead());
                mEntity |= mask;
                getBlock()->incRef();
            }
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

        bool EntityPtr::operator==(Entity *other) const
        {
            update();
            return get() == other;
        }

        std::strong_ordering EntityPtr::operator<=>(const EntityPtr &other) const
        {
            update();
            other.update();
            return mEntity <=> other.mEntity;
        }

        bool EntityPtr::isDead() const
        {
            return !holdsRef() || getBlock()->dead();
        }

        Serialize::StreamResult EntityPtr::readState(Serialize::SerializeInStream &in, const char *name)
        {
            Entity *ptr;
            STREAM_PROPAGATE_ERROR(Serialize::read(in, ptr, name));
            mEntity = reinterpret_cast<uintptr_t>(ptr);
            return {};
        }

        void EntityPtr::writeState(Serialize::SerializeOutStream &out, const char *name) const
        {
            Serialize::write(out, get(), name);
        }

        void EntityPtr::applySerializableMap(Serialize::SerializeInStream &in, bool success)
        {
            if (mEntity & static_cast<uintptr_t>(Serialize::UnitIdTag::SYNCABLE)) {
                Entity *ptr = reinterpret_cast<Entity *>(mEntity);
                Serialize::UnitHelper<Entity *>::applyMap(in, ptr, success);
                *this = ptr;
            }
        }

        bool EntityPtr::holdsRef() const
        {
            return mEntity & mask;
        }

        typename EntityPtr::ControlBlockDummy EntityPtr::getBlock() const
        {
            return { reinterpret_cast<SceneManager::ControlBlock *>(mEntity & ~mask) };
        }
    }
}
}