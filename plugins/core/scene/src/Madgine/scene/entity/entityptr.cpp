#include "../../scenelib.h"

#include "entity.h"
#include "entityptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        EntityPtr::EntityPtr(const EntityPtr &other)
            : mIndex(other.mSceneMgr->entities().copy(other.mIndex))
            , mSceneMgr(other.mSceneMgr)
        {
        }

        EntityPtr::EntityPtr(const typename GenerationVector<Entity>::iterator &it)
            : mIndex(it.copyIndex())
            , mSceneMgr(it.valid() ? &it->sceneMgr() : nullptr)
        {
        }

        EntityPtr &EntityPtr::operator=(const EntityPtr &other)
        {
            if (mIndex)
                mSceneMgr->entities().reset(mIndex);
            if (other.mIndex)
                mIndex = other.mSceneMgr->entities().copy(other.mIndex);
            mSceneMgr = other.mSceneMgr;
            return *this;
        }

        EntityPtr::~EntityPtr()
        {
            mSceneMgr->entities().reset(mIndex);
        }

        Entity *EntityPtr::operator->() const
        {
            return &mSceneMgr->entities()[mIndex];
        }

        SceneManager *EntityPtr::sceneMgr() const
        {
            return mSceneMgr;
        }

        void EntityPtr::update() const
        {
            return mSceneMgr->entities().update(mIndex);
        }

        EntityPtr::operator bool() const
        {
            update();
            return mIndex;
        }

        EntityPtr::operator Entity *() const
        {
            return &mSceneMgr->entities()[mIndex];
        }

        bool EntityPtr::operator==(const EntityPtr &other) const
        {
            assert(mSceneMgr == other.mSceneMgr || !mSceneMgr || !other.mSceneMgr);
            update();
            other.update();
            return mIndex == other.mIndex;
        }

        bool EntityPtr::operator!=(const typename GenerationVector<Entity>::iterator &it) const
        {
            update();
            if (!mIndex || !it.valid())
                return it.valid() != mIndex;
            assert(&mSceneMgr->entities() == it.mVector);
            return mIndex != it.mIndex;
        }

        EntityPtr &EntityPtr::operator++()
        {
            //TODO: physical() is not good here
            mSceneMgr->entities().increment(mIndex, physical(mSceneMgr->entities()).size());
            return *this;
        }

        EntityComponentPtr<EntityComponentBase> EntityPtr::getComponent(size_t i) const
        {
            return (*this)->getComponent(i, *this);
        }

        Future<EntityComponentPtr<EntityComponentBase>> EntityPtr::addComponent(size_t i, const ObjectPtr &table) const
        {
            return (*this)->addComponent(i, *this, table);
        }

        Future<EntityComponentPtr<EntityComponentBase>> EntityPtr::addComponent(const std::string_view &name, const ObjectPtr &table) const
        {
            return (*this)->addComponent(name, *this, table);
        }

        GenerationVector<Entity>::iterator EntityPtr::it()
        {
            return { mSceneMgr->entities().copy(mIndex), &mSceneMgr->entities() };
        }

        void EntityPtr::remove()
        {
            mSceneMgr->remove(*this);
        }

    }
}
}