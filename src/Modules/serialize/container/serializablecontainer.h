#pragma once

#include "../../generic/container/container_api.h"
#include "../../generic/copy_traits.h"
#include "../../generic/noopfunctor.h"
#include "../../generic/container/observerevent.h"
#include "../../generic/offsetptr.h"
#include "../serializable.h"
#include "../unithelper.h"

namespace Engine {
namespace Serialize {

    template <typename C, typename Observer = NoOpFunctor, typename controlled = std::false_type, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    struct SerializableContainerImpl : Serializable<OffsetPtr>,
                                       CopyTraits<typename container_traits<C>::value_type>,
                                       C,
                                       Observer {        

        using _traits = container_traits<C>;
        using container = SerializableContainerImpl<C, Observer, controlled, OffsetPtr>;

        typedef typename _traits::container Base;
        typedef typename _traits::iterator iterator;
        typedef typename _traits::const_iterator const_iterator;
        typedef typename _traits::reverse_iterator reverse_iterator;
        typedef typename _traits::const_reverse_iterator const_reverse_iterator;
        typedef typename _traits::position_handle position_handle;

        typedef typename _traits::value_type value_type;

        typedef UnitHelper<C> ContainerUnitHelper;
        typedef UnitHelper<value_type> ItemUnitHelper;

        SerializableContainerImpl()
            : mActiveIterator(_traits::toPositionHandle(*this, Base::begin()))
        {
        }

        SerializableContainerImpl(const Base &c)
            : Base(c)
            , mActiveIterator(_traits::toPositionHandle(*this, Base::begin()))
        {
        }

        SerializableContainerImpl(const SerializableContainerImpl &other)
            : Base(other)
            , mActiveIterator(_traits::toPositionHandle(*this, Base::begin()))
        {
        }

        SerializableContainerImpl(SerializableContainerImpl &&other)
        {
            if (other.isSynced()) {
                ContainerUnitHelper::setItemDataSynced(other, false);
            }
            Base::operator=(std::move(other));
            mActiveIterator = std::move(other.mActiveIterator);
            ContainerUnitHelper::setItemParent(*this, OffsetPtr::parent(this));
            other.Base::clear();
            other.mActiveIterator = _traits::toPositionHandle(other, other.Base::begin());
        }

        SerializableContainerImpl<C, Observer, controlled, OffsetPtr> &operator=(const Base &other)
        {
            ResetOperation { *this } = other;
            return *this;
        }

        Observer &observer()
        {
            return *this;
        }

        iterator begin()
        {
            return Base::begin();
        }

        const_iterator begin() const
        {
            return Base::begin();
        }

        iterator end()
        {
            return _traits::toIterator(*this, mActiveIterator);
        }

        const_iterator end() const
        {
            return _traits::toIterator(*this, mActiveIterator);
        }

        reverse_iterator rbegin()
        {
            return _traits::toIterator(*this, mActiveIterator);
        }

        const_reverse_iterator rbegin() const
        {
            return _traits::toIterator(*this, mActiveIterator);
        }

        reverse_iterator rend()
        {
            return Base::rend();
        }

        const_reverse_iterator rend() const
        {
            return Base::rend();
        }

        size_t size() const
        {
            return std::distance(Base::begin(), _traits::toIterator(*this, mActiveIterator));
        }

        void clear()
        {
            ResetOperation { *this }.clear();
        }

        template <typename... _Ty>
        typename _traits::emplace_return emplace(const iterator &where, _Ty &&... args)
        {
            return InsertOperation { *this, where }.emplace(where, std::forward<_Ty>(args)...);
        }

        template <typename T, typename... _Ty>
        typename _traits::emplace_return emplace_init(T &&init, const iterator &where, _Ty &&... args)
        {
            InsertOperation op { *this, where };
            typename _traits::emplace_return it = op.emplace(where, std::forward<_Ty>(args)...);
            if (_traits::was_emplace_successful(it)) {
                init(*it);
            }
            return it;
        }

        iterator erase(const iterator &where)
        {
            return RemoveOperation { *this, where }.erase(where);
        }

        iterator erase(const iterator &from, const iterator &to)
        {
            return RemoveRangeOperation { *this, from, to }.erase(from, to);
        }

        value_type extract(const iterator &which)
        {
            RemoveOperation op { *this, which };
            value_type temp = std::move(*which);
            op.erase(which);
            return temp;
        }

        void setActive(bool active, bool existenceChange)
        {
            if (active) {
                while (mActiveIterator != _traits::toPositionHandle(*this, Base::end())) {
                    iterator it = _traits::toIterator(*this, mActiveIterator);
                    ++mActiveIterator;
                    if (existenceChange) {
                        Observer::operator()(it, BEFORE | INSERT_ITEM);
                        Observer::operator()(it, AFTER | INSERT_ITEM);
                    }
                    Observer::operator()(it, BEFORE | ACTIVATE_ITEM);
                    ItemUnitHelper::setItemActive(*it, active, existenceChange || !controlled::value);
                    Observer::operator()(it, AFTER | ACTIVATE_ITEM);
                }
            } else {
                while (mActiveIterator != _traits::toPositionHandle(*this, Base::begin())) {
                    --mActiveIterator;
                    iterator it = _traits::toIterator(*this, mActiveIterator);
                    Observer::operator()(it, BEFORE | DEACTIVATE_ITEM);
                    ItemUnitHelper::setItemActive(*it, active, existenceChange || !controlled::value);
                    Observer::operator()(it, AFTER | DEACTIVATE_ITEM);
                    if (existenceChange) {
                        Observer::operator()(it, BEFORE | REMOVE_ITEM);
                        Observer::operator()(it, AFTER | REMOVE_ITEM);
                    }
                }
            }
        }

        const C &physical() const
        {
            return *this;
        }

        C &physical()
        {
            return *this;
        }

        template <typename T>
        iterator find(T &&t)
        {
            iterator it = Base::find(std::forward<T>(t));
            if (it == Base::end())
                it = end();
            return it;
        }

        template <typename T>
        const_iterator find(T &&t) const
        {
            const_iterator it = Base::find(std::forward<T>(t));
            if (it == Base::end())
                it = end();
            return it;
        }

        struct Operation {

            Operation(container &c)
                : mContainer(c)
            {
            }

            size_t size()
            {
                return mContainer.Base::size();
            }

        protected:
            container &mContainer;
        };

        struct EmplaceOperation : Operation {
            using Operation::Operation;

            template <typename... Ty>
            auto emplace(const const_iterator &it, Ty &&... args)
            {
                ++mTriggered;
                auto retIt = this->mContainer.emplace_intern(it, std::forward<Ty>(args)...);
                mLastIt = retIt;
                mLastInserted = _traits::was_emplace_successful(retIt);
                return retIt;
            }

        protected:
            int mTriggered = 0;
            bool mLastInserted;
            iterator mLastIt;
        };

        /*struct EraseOperation : Operation {
            using Operation::Operation;


        };*/

        struct InsertOperation : EmplaceOperation {
            InsertOperation(container &c, const iterator &it)
                : EmplaceOperation(c)
            {
                if (this->mContainer.isItemActive(it)) {
                    this->mContainer.Observer::operator()(it, BEFORE | INSERT_ITEM);
                }
            }

            ~InsertOperation()
            {
                assert(this->mTriggered == 1);
                if (this->mLastInserted) {
                    if (this->mContainer.isSynced())
                        ItemUnitHelper::setItemDataSynced(*this->mLastIt, true);
                }
                if (this->mContainer.isItemActive(this->mLastIt)) {
                    this->mContainer.Observer::operator()(this->mLastIt, (this->mLastInserted ? AFTER : ABORTED) | INSERT_ITEM);
                    if (this->mLastInserted) {
                        this->mContainer.Observer::operator()(this->mLastIt, BEFORE | ACTIVATE_ITEM);
                        ItemUnitHelper::setItemActive(*this->mLastIt, true, true);
                        this->mContainer.Observer::operator()(this->mLastIt, AFTER | ACTIVATE_ITEM);
                    }
                }
            }
        };

        struct RemoveOperation : Operation {
            RemoveOperation(container &c, const iterator &it)
                : Operation(c)
            {
                if (this->mContainer.isSynced()) {
                    ItemUnitHelper::setItemDataSynced(*it, false);
                }
                if (this->mContainer.isItemActive(it)) {
                    this->mContainer.Observer::operator()(it, BEFORE | DEACTIVATE_ITEM);
                    ItemUnitHelper::setItemActive(*it, false, true);
                    this->mContainer.Observer::operator()(it, AFTER | DEACTIVATE_ITEM);
                    this->mContainer.Observer::operator()(it, BEFORE | REMOVE_ITEM);
                    mWasActive = true;
                }
            }

            ~RemoveOperation()
            {
                assert(mTriggered);
                if (mWasActive) {
                    this->mContainer.Observer::operator()(mIt, AFTER | REMOVE_ITEM);
                }
            }

            auto erase(const iterator &it)
            {
                assert(!mTriggered);
                mTriggered = true;
                mIt = this->mContainer.erase_intern(it);
                return mIt;
            }

        private:
            bool mWasActive = false;
            bool mTriggered = false;
            iterator mIt;
        };

        struct RemoveRangeOperation : Operation {
            RemoveRangeOperation(container &c, const iterator &from, const iterator &to)
                : Operation(c)
            {
                if (this->mContainer.isSynced()) {
                    for (iterator it = from; it != to; ++it) {
                        ItemUnitHelper::setItemDataSynced(*it, false);
                    }
                }
                for (iterator it = from; it != to && this->mContainer.isItemActive(it); ++it) {
                    this->mContainer.Observer::operator()(it, BEFORE | DEACTIVATE_ITEM);
                    ItemUnitHelper::setItemActive(*it, false, true);
                    this->mContainer.Observer::operator()(it, AFTER | DEACTIVATE_ITEM);
                    this->mContainer.Observer::operator()(it, BEFORE | REMOVE_ITEM);
                    ++mCount;
                }
            }

            ~RemoveRangeOperation()
            {
                assert(mTriggered);
                for (size_t i = 0; i < mCount; ++i) {
                    this->mContainer.Observer::operator()(mIt, AFTER | REMOVE_ITEM);
                }
            }

            auto erase(const iterator &from, const iterator &to)
            {
                assert(!mTriggered);
                mTriggered = true;
                mIt = this->mContainer.erase_intern(from, to);
                return mIt;
            }

        private:
            size_t mCount = 0;
            bool mTriggered = false;
            iterator mIt;
        };

        struct ResetOperation : EmplaceOperation {
            ResetOperation(container &c)
                : EmplaceOperation(c)
            {
                if (this->mContainer.isSynced()) {
                    ContainerUnitHelper::setItemDataSynced(this->mContainer, false);
                }

                if (this->mContainer.isActive()) {
                    assert(this->mContainer.mActiveIterator == _traits::toPositionHandle(this->mContainer, this->mContainer.Base::end()));
                    this->mContainer.Observer::operator()(this->mContainer.end(), BEFORE | RESET);
                    this->mContainer.setActive(false, !controlled::value);
                    mWasActive = true;
                }
            }
            ~ResetOperation()
            {
                if (this->mContainer.isSynced()) {
                    ContainerUnitHelper::setItemDataSynced(this->mContainer, true);
                }
                if (mWasActive) {
                    this->mContainer.setActive(true, !controlled::value);
                    this->mContainer.Observer::operator()(this->mContainer.end(), AFTER | RESET);
                }
            }

            void clear()
            {
                this->mContainer.clear_intern();
            }

            template <typename T>
            void operator=(T &&other)
            {
                this->mContainer.Base::operator=(std::forward<T>(other));
            }

        private:
            bool mWasActive = false;
        };

    protected:
        bool isItemActive(const iterator &it)
        {
            typename Base::iterator active = _traits::toIterator(*this, mActiveIterator);
            if (active == Base::end())
                return this->isActive();
            for (auto it2 = Base::begin(); it2 != active; ++it2) {
                if (it2 == it)
                    return true;
            }
            return false;
        }

        template <typename... _Ty>
        typename _traits::emplace_return emplace_intern(const const_iterator &where, _Ty &&... args)
        {
            typename _traits::emplace_return it = _traits::emplace(*this, where, std::forward<_Ty>(args)...);
            if (_traits::was_emplace_successful(it)) {
                _traits::revalidateHandleAfterInsert(mActiveIterator, *this, { it });
                position_handle newHandle = _traits::toPositionHandle(*this, it);
                if (_traits::next(newHandle) == mActiveIterator && !this->isActive())
                    mActiveIterator = newHandle;
                ItemUnitHelper::setItemParent(*it, OffsetPtr::parent(this));
            }
            return it;
        }

        iterator erase_intern(const iterator &it)
        {
            bool test = mActiveIterator == _traits::toPositionHandle(*this, it);
            iterator newIt = Base::erase(it);
            if (test)
                mActiveIterator = _traits::toPositionHandle(*this, newIt);
            else
                _traits::revalidateHandleAfterRemove(mActiveIterator, *this, newIt);
            return newIt;
        }

        iterator erase_intern(const iterator &from, const iterator &to)
        {
            bool test = false;
            size_t count = 0;
            for (iterator it = from; it != to; ++it) {
                if (mActiveIterator == _traits::toPositionHandle(*this, it))
                    test = true;
                ++count;
            }
            iterator newIt = Base::erase(from, to);
            if (test)
                mActiveIterator = _traits::toPositionHandle(*this, newIt);
            else
                _traits::revalidateHandleAfterRemove(mActiveIterator, *this, newIt, count);
            return newIt;
        }

        void clear_intern()
        {
            Base::clear();
            mActiveIterator = _traits::toPositionHandle(*this, Base::begin());
        }

    protected:
        position_handle mActiveIterator;
    };

    template <typename C, typename Observer = NoOpFunctor, typename controlled = std::false_type, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    using SerializableContainer = container_api<SerializableContainerImpl<C, Observer, controlled, OffsetPtr>>;

#define SERIALIZABLE_CONTAINER(Name, ...) OFFSET_CONTAINER(Name, ::Engine::Serialize::SerializableContainer<__VA_ARGS__>)

}

template <typename C, typename Observer, typename controlled, typename _OffsetPtr>
struct underlying_container<Serialize::SerializableContainerImpl<C, Observer, controlled, _OffsetPtr>> {
    typedef C type;
};


template <typename C, typename Observer, typename controlled, typename _OffsetPtr>
struct container_traits<Serialize::SerializableContainerImpl<C, Observer, controlled, _OffsetPtr>> : container_traits<C> {
    typedef Serialize::SerializableContainerImpl<C, Observer, controlled, _OffsetPtr> container;

    using _traits = container_traits<C>;

    template <typename... Args>
    static typename _traits::emplace_return emplace(container &c, const typename _traits::iterator &where, Args &&... args)
    {
        return c.emplace(where, std::forward<Args>(args)...);
    }
};


}
