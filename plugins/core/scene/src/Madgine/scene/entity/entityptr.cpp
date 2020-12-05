#include "../../scenelib.h"

#include "entity.h"
#include "entityptr.h"

#include "../scenemanager.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        EntityPtr::EntityPtr(const EntityPtr &other)
            : mHandle { other.mSceneMgr->entities().copy(other.mHandle.mIndex) }
            , mSceneMgr(other.mSceneMgr)
        {
        }

        EntityPtr::EntityPtr(const typename GenerationVector<Entity>::iterator &it)
            : mHandle { it.copyIndex() }
            , mSceneMgr(it.valid() ? &it->sceneMgr() : nullptr)
        {
        }

        EntityPtr &EntityPtr::operator=(const EntityPtr &other)
        {
            if (mHandle.mIndex)
                mSceneMgr->entities().reset(mHandle.mIndex);
            if (other.mHandle.mIndex)
                mHandle.mIndex = other.mSceneMgr->entities().copy(other.mHandle.mIndex);
            mSceneMgr = other.mSceneMgr;
            return *this;
        }

        EntityPtr &EntityPtr::operator=(EntityPtr &&other)
        {
            mHandle = std::move(other.mHandle);
            std::swap(mSceneMgr, other.mSceneMgr);
            return *this;
        }

        EntityPtr::~EntityPtr()
        {
            mSceneMgr->entities().reset(mHandle.mIndex);
        }

        Entity *EntityPtr::operator->() const
        {
            return &mSceneMgr->entities()[mHandle.mIndex];
        }

        SceneManager *EntityPtr::sceneMgr() const
        {
            return mSceneMgr;
        }

        uint32_t EntityPtr::update() const
        {
            return mSceneMgr->entities().update(mHandle.mIndex);
        }

        EntityPtr::operator bool() const
        {
            update();
            return mHandle.mIndex;
        }

        EntityPtr::operator Entity *() const
        {
            return get();
        }

        Entity *EntityPtr::get() const
        {
            return mHandle.mIndex ? &mSceneMgr->entities()[mHandle.mIndex] : nullptr;
        }

        bool EntityPtr::operator==(const EntityPtr &other) const
        {
            assert(mSceneMgr == other.mSceneMgr || !mSceneMgr || !other.mSceneMgr);
            update();
            other.update();
            return mHandle == other.mHandle;
        }

        bool EntityPtr::operator!=(const typename GenerationVector<Entity>::iterator &it) const
        {
            update();
            if (!mHandle.mIndex || !it.valid())
                return it.valid() != mHandle.mIndex;
            assert(&mSceneMgr->entities() == it.mVector);
            return mHandle.mIndex != it.mIndex;
        }

        EntityPtr &EntityPtr::operator++()
        {
            //TODO: physical() is not good here
            mSceneMgr->entities().increment(mHandle.mIndex, physical(mSceneMgr->entities()).size());
            return *this;
        }

        EntityComponentPtr<EntityComponentBase> EntityPtr::getComponent(size_t i) const
        {
            return mHandle.mIndex ? get()->getComponent(i, *this) : EntityComponentPtr<EntityComponentBase>{};
        }

        Future<EntityComponentPtr<EntityComponentBase>> EntityPtr::addComponent(size_t i, const ObjectPtr &table) const
        {
            return get()->addComponent(i, *this, table);
        }

        Future<EntityComponentPtr<EntityComponentBase>> EntityPtr::addComponent(const std::string_view &name, const ObjectPtr &table) const
        {
            return get()->addComponent(name, *this, table);
        }

        bool EntityPtr::hasComponent(size_t i) const
        {
            return get()->hasComponent(i);
        }

        EntityPtr::operator EntityHandle() const
        {
            return { mHandle.mIndex ? mSceneMgr->entities().copy(mHandle.mIndex) : GenerationContainerIndex {} };
        }

        GenerationVector<Entity>::iterator EntityPtr::it() const
        {
            return { mSceneMgr->entities().copy(mHandle.mIndex), &mSceneMgr->entities() };
        }

        void EntityPtr::remove() const
        {
            mSceneMgr->remove(*this);
        }

    }
}
}