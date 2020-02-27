#pragma once

#include "../../generic/container_traits.h"
#include "../../generic/noopfunctor.h"
#include "../../generic/observerevent.h"
#include "../../generic/offsetptr.h"
#include "../../generic/tupleunpacker.h"
#include "../serializable.h"
#include "../streams/serializestream.h"
#include "../creationhelper.h"
#include "../unithelper.h"

namespace Engine {
namespace Serialize {
    template <typename C, typename Observer = NoOpFunctor, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    struct SerializableContainerImpl : Serializable<OffsetPtr>,
                                       protected UnitHelper<typename container_traits<C>::value_type>,
                                       C,
                                       Observer {

        using _traits = container_traits<C>;

        struct traits : _traits {

            typedef SerializableContainerImpl<C, Observer, OffsetPtr> container;

            template <typename... _Ty>
            static std::pair<typename _traits::iterator, bool> emplace(container &c, const typename _traits::iterator &where, _Ty &&... args)
            {
                return c.emplace(where, std::forward<_Ty>(args)...);
            }
        };

        typedef typename _traits::container Base;
        typedef typename _traits::iterator iterator;
        typedef typename _traits::const_iterator const_iterator;
        typedef typename _traits::reverse_iterator reverse_iterator;
        typedef typename _traits::const_reverse_iterator const_reverse_iterator;
        typedef typename _traits::position_handle position_handle;

        typedef typename _traits::value_type value_type;

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
                other.setDataSynced(false);
            }
            Base::operator=(std::move(other));
            mActiveIterator = std::move(other.mActiveIterator);
            for (auto &t : physical())
                this->setParent(t, OffsetPtr::parent(this));
            other.Base::clear();
            other.mActiveIterator = _traits::toPositionHandle(other, other.Base::begin());
        }

        SerializableContainerImpl<C, Observer, OffsetPtr> &operator=(const Base &other)
        {
            bool wasActive = beforeReset();
            Base::operator=(other);
            mActiveIterator = _traits::toPositionHandle(*this, Base::begin());
            afterReset(wasActive);
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
            bool wasActive = beforeReset();
            Base::clear();
            mActiveIterator = _traits::toPositionHandle(*this, Base::begin());
            afterReset(wasActive);
        }

        template <typename... _Ty>
        std::pair<iterator, bool> emplace(const iterator &where, _Ty &&... args)
        {
            beforeInsert(where);
            std::pair<iterator, bool> it = emplace_intern(where, std::forward<_Ty>(args)...);
            afterInsert(it.second, it.first);
            return it;
        }

        template <typename T, typename... _Ty>
        std::pair<iterator, bool> emplace_init(T &&init, const iterator &where, _Ty &&... args)
        {
            beforeInsert(where);
            std::pair<iterator, bool> it = emplace_intern(where, std::forward<_Ty>(args)...);
            if (it.second) {
                init(*it.first);
            }
            afterInsert(it.second, it.first);
            return it;
        }

        iterator erase(const iterator &where)
        {
            bool b = beforeRemove(where);
            iterator it = erase_intern(where);
            afterRemove(b, it);
            return it;
        }

        iterator erase(const iterator &from, const iterator &to)
        {
            size_t count = beforeRemoveRange(from, to);
            iterator it = erase_intern(from, to);
            afterRemoveRange(count, it);
            return it;
        }

        value_type extract(const iterator &which)
        {
            bool b = beforeRemove(which);
            value_type temp = std::move(*which);
            iterator it = erase_intern(which);
            afterRemove(b, it);
            return temp;
        }

        std::pair<iterator, bool> read_item_where(SerializeInStream &in, const const_iterator &where)
        {
            beforeInsert(where);
            std::pair<iterator, bool> it = read_item_where_intern(in, where);
            afterInsert(it.second, it.first);
            return it;
        }

        void writeState(SerializeOutStream &out, const char *name = nullptr) const
        {
            if (name)
                out.format().beginExtendedCompound(out, name);
            out.write(Base::size(), "size");
            if (name)
                out.format().beginCompound(out, name);
            for (const auto &t : physical()) {
                if (this->filter(out, t)) {
                    write_item(out, t);
                }
            }
            if (name)
                out.format().endCompound(out, name);
        }

        template <typename Creator = DefaultCreator<>>
        void readState(SerializeInStream &in, const char *name = nullptr, Creator &&creator = {})
        {
            bool wasActive = beforeReset();
            readState_intern(in, name, std::forward<Creator>(creator));
            afterReset(wasActive);
        }

        void applySerializableMap(SerializeInStream &in)
        {
            for (value_type &t : physical()) {
                this->applyMap(in, t);
            }
        }

        void setDataSynced(bool b)
        {
            for (value_type &t : physical()) {
                this->setItemDataSynced(t, b);
            }
        }

        void setActive(bool active, bool existenceChange, bool controlled = false)
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
                    this->setItemActive(*it, active, existenceChange || !controlled);
                    Observer::operator()(it, AFTER | ACTIVATE_ITEM);
                }
            } else {
                while (mActiveIterator != _traits::toPositionHandle(*this, Base::begin())) {
                    --mActiveIterator;
                    iterator it = _traits::toIterator(*this, mActiveIterator);
                    Observer::operator()(it, BEFORE | DEACTIVATE_ITEM);
                    this->setItemActive(*it, active, existenceChange || !controlled);
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

    protected:
        void beforeInsert(const iterator &it)
        {
            if (isItemActive(it)) {
                Observer::operator()(it, BEFORE | INSERT_ITEM);
            }
        }

        void afterInsert(bool inserted, const iterator &it)
        {
            if (inserted) {
                if (this->isSynced())
                    this->setItemDataSynced(*it, true);
            }
            if (isItemActive(it)) {
                Observer::operator()(it, (inserted ? AFTER : ABORTED) | INSERT_ITEM);
                if (inserted) {
                    Observer::operator()(it, BEFORE | ACTIVATE_ITEM);
                    this->setItemActive(*it, true, true);
                    Observer::operator()(it, AFTER | ACTIVATE_ITEM);
                }
            }
        }

        bool beforeRemove(const iterator &it)
        {
            if (this->isSynced()) {
                this->setItemDataSynced(*it, false);
            }
            if (isItemActive(it)) {
                Observer::operator()(it, BEFORE | DEACTIVATE_ITEM);
                this->setItemActive(*it, false, true);
                Observer::operator()(it, AFTER | DEACTIVATE_ITEM);
                Observer::operator()(it, BEFORE | REMOVE_ITEM);
                return true;
            }
            return false;
        }

        void afterRemove(bool b, const iterator &it)
        {
            if (b) {
                Observer::operator()(it, AFTER | REMOVE_ITEM);
            }
        }

        size_t beforeRemoveRange(const iterator &from, const iterator &to)
        {
            if (this->isSynced()) {
                for (iterator it = from; it != to; ++it) {
                    this->setItemDataSynced(*it, false);
                }
            }
            size_t count = 0;
            for (iterator it = from; it != to && isItemActive(it); ++it) {
                Observer::operator()(it, BEFORE | DEACTIVATE_ITEM);
                this->setItemActive(*it, false, true);
                Observer::operator()(it, AFTER | DEACTIVATE_ITEM);
                Observer::operator()(it, BEFORE | REMOVE_ITEM);
                ++count;
            }
            return count;
        }

        void afterRemoveRange(size_t count, const iterator &it)
        {
            for (size_t i = 0; i < count; ++i) {
                Observer::operator()(it, AFTER | REMOVE_ITEM);
            }
        }

        bool beforeReset(bool controlled = false)
        {
            if (this->isSynced()) {
                setDataSynced(false);
            }

            if (this->isActive()) {
                assert(mActiveIterator == _traits::toPositionHandle(*this, Base::end()));
                Observer::operator()(end(), BEFORE | RESET);
                setActive(false, !controlled);
                return true;
            }
            return false;
        }

        void afterReset(bool wasActive, bool controlled = false)
        {
            if (this->isSynced()) {
                setDataSynced(true);
            }
            if (wasActive) {
                setActive(true, !controlled);
                Observer::operator()(end(), AFTER | RESET);
            }
        }

        template <typename Creator = DefaultCreator<>>
        void readState_intern(SerializeInStream &in, const char *name = nullptr, Creator &&creator = {})
        {
            Base::clear();
            mActiveIterator = _traits::toPositionHandle(*this, Base::begin());
            if (name)
                in.format().beginExtendedCompound(in, name);
            decltype(Base::size()) count;
            in.read(count, "size");
            if (name)
                in.format().beginCompound(in, name);
            while (count--) {
                this->read_item_where_intern(in, Base::end(), std::forward<Creator>(creator));
            }
            if (name)
                in.format().endCompound(in, name);
        }

        template <typename Creator>
        std::pair<iterator, bool> read_item_where_intern(SerializeInStream &in, const const_iterator &where, Creator &&creator)
        {
            this->beginExtendedItem(in, nullref<const value_type>);
            std::pair<iterator, bool> it;
            if constexpr (std::is_const_v<value_type>) {
                std::remove_const_t<value_type> temp = TupleUnpacker::constructFromTuple<std::remove_const_t<value_type>>(creator.readCreationData(in));
                in.read(temp, "Item");
                it = emplace_intern(where, std::move(temp));
            } else {
                it = TupleUnpacker::invokeExpand(LIFT_MEMBER(emplace_intern), this, where, creator.readCreationData(in));
                in.read(*it.first, "Item");
            }
            assert(it.second);
            return it;
        }

        void write_item(SerializeOutStream &out, const value_type &t) const
        {
            this->beginExtendedItem(out, t);
            this->write_creation(out, t);
            out.write(t, "Item");
        }

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
        std::pair<iterator, bool> emplace_intern(const const_iterator &where, _Ty &&... args)
        {
            std::pair<iterator, bool> it = _traits::emplace(*this, where, std::forward<_Ty>(args)...);
            if (it.second) {
                _traits::revalidateHandleAfterInsert(mActiveIterator, *this, it.first);
                position_handle newHandle = _traits::toPositionHandle(*this, it.first);
                if (_traits::next(newHandle) == mActiveIterator && !this->isActive())
                    mActiveIterator = newHandle;
                this->setParent(*it.first, OffsetPtr::parent(this));
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

    protected:
        position_handle mActiveIterator;
    };

    template <typename C, typename Observer = NoOpFunctor, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    using SerializableContainer = typename container_traits<C>::template api<SerializableContainerImpl<C, Observer, OffsetPtr>>;

#define SERIALIZABLE_CONTAINER(Name, ...) OFFSET_CONTAINER(Name, ::Engine::Serialize::SerializableContainer<__VA_ARGS__>)

}
}
