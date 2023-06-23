#include "../../scenelib.h"

#include "entity.h"
#include "entityptr.h"

#include "../scenemanager.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/serialize/operations.h"

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

        EntityPtr::EntityPtr(const EntityPtr &other)
            : mEntity(other.mEntity)
        {
            mHoldsRef = other.mHoldsRef;
            if (mHoldsRef)
                getBlock()->incRef();
        }

        EntityPtr::~EntityPtr()
        {
            reset();
        }

        EntityPtr::EntityPtr(Entity *entity)
            : mEntity(entity)
        {
            if (mEntity) {
                mHoldsRef = true;
                getBlock()->incRef();
                assert(!getBlock()->dead());
            }
        }

        EntityPtr &EntityPtr::operator=(const EntityPtr &other)
        {
            if (mHoldsRef)
                getBlock()->decRef();
            mEntity = other.mEntity;
            mHoldsRef = other.mHoldsRef;
            if (mHoldsRef)
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
            if (mHoldsRef && getBlock()->dead()) {
                getBlock()->decRef();
                mHoldsRef = false;
            }
        }

        void EntityPtr::reset()
        {
            if (mHoldsRef) {
                getBlock()->decRef();
                mHoldsRef = false;
            }
            mEntity = nullptr;
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
            return isDead() ? nullptr : getBlock()->get();
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
            return !mHoldsRef || getBlock()->dead();
        }

        typename EntityPtr::ControlBlockDummy EntityPtr::getBlock() const
        {
            assert(mHoldsRef);
            return { SceneManager::ControlBlock::fromPtr(mEntity) };
        }
    }
}
namespace Serialize {

    StreamResult Operations<Scene::Entity::EntityPtr>::read(Serialize::FormattedSerializeStream &in, Scene::Entity::EntityPtr &ptr, const char *name)
    {
        Scene::Entity::Entity *dummy;
        STREAM_PROPAGATE_ERROR(Serialize::read(in, dummy, name));
        reinterpret_cast<uintptr_t&>(ptr) = reinterpret_cast<uintptr_t>(dummy);
        return {};
    }

    void Operations<Scene::Entity::EntityPtr>::write(FormattedSerializeStream &out, const Scene::Entity::EntityPtr &ptr, const char *name)
    {
        Serialize::write(out, ptr.get(), name);
    }

    StreamResult Operations<Scene::Entity::EntityPtr>::applyMap(Serialize::FormattedSerializeStream &in, Scene::Entity::EntityPtr &ptr, bool success)
    {
        uintptr_t v = reinterpret_cast<uintptr_t&>(ptr);
        if (v & static_cast<uintptr_t>(UnitIdTag::SYNCABLE)) {
            Scene::Entity::Entity *dummy = reinterpret_cast<Scene::Entity::Entity *>(v);
            STREAM_PROPAGATE_ERROR(Serialize::applyMap(in, dummy, success));
            ptr = dummy;
        }
        return {};
    }

}
}