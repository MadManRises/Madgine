#pragma once

#include "../../generic/tupleunpacker.h"
#include "../../keyvalue/container_traits.h"
#include "../serializable.h"
#include "../streams/serializestream.h"
#include "creationhelper.h"
#include "offset.h"
#include "unithelper.h"

namespace Engine {
namespace Serialize {
    template <typename OffsetPtr, typename C>
    struct SerializableContainerImpl : Serializable<OffsetPtr>,
                                       protected UnitHelper<typename container_traits<C>::type>,
                                       container_traits<C>::template rebind<UnitHelper_t>::container {

        using _traits = typename container_traits<C>::template rebind<UnitHelper_t>;

        struct traits : _traits {

            typedef SerializableContainerImpl<OffsetPtr, C> container;

            template <class... _Ty>
            static std::pair<typename _traits::iterator, bool> emplace(container &c, const typename _traits::const_iterator &where, _Ty &&... args)
            {
                return c.emplace(where, std::forward<_Ty>(args)...);
            }
        };

        typedef typename _traits::container Base;
        typedef typename _traits::iterator iterator;
        typedef typename _traits::const_iterator const_iterator;

        typedef typename _traits::type type;

        SerializableContainerImpl()
            : mActiveIterator(Base::begin())
        {
        }

        SerializableContainerImpl(const Base &c)
            : Base(c)
            , mActiveIterator(Base::begin())
        {
        }

        SerializableContainerImpl(const SerializableContainerImpl &other)
            : Base(other)
            , mActiveIterator(Base::begin())
        {
        }

        SerializableContainerImpl(SerializableContainerImpl &&other)
        {
            if (other.isSynced()) {
                other.setDataSynced(false);
            }
            Base::operator=(std::forward<Base>(other));
            for (auto &t : *this)
                this->setParent(t, OffsetPtr::parent(this));
            mActiveIterator = other.mActiveIterator;
            Base::clear();
            other.mActiveIterator = other.Base::begin();
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
            return Base::end();
        }

        const_iterator end() const
        {
            return Base::end();
        }

        using Base::empty;
        using Base::size;

        bool operator==(const SerializableContainerImpl<OffsetPtr, C> &other) const
        {
            return static_cast<const Base &>(*this) == other;
        }

        SerializableContainerImpl<OffsetPtr, C> &operator=(const Base &other)
        {
            bool wasActive = beforeReset();
            Base::operator=(other);
            mActiveIterator = Base::begin();
            afterReset(wasActive);
            return *this;
        }

        void clear()
        {
            bool wasActive = beforeReset();
            Base::clear();
            mActiveIterator = Base::begin();
            afterReset(wasActive);
        }

        template <class... _Ty>
        std::pair<iterator, bool> emplace(const const_iterator &where, _Ty &&... args)
        {
            std::pair<iterator, bool> it = emplace_intern(where, std::forward<_Ty>(args)...);
            if (it.second)
                afterInsert(it.first);
            return it;
        }

        template <class T, class... _Ty>
        std::pair<iterator, bool> emplace_init(T &&init, const const_iterator &where, _Ty &&... args)
        {
            std::pair<iterator, bool> it = emplace_intern(where, std::forward<_Ty>(args)...);
            if (it.second) {
                init(*it.first);
                onInsert(it.first);
            }
            return it;
        }

        iterator erase(const iterator &where)
        {
            beforeRemove(where);
            return erase_intern(where);
        }

        iterator erase(const iterator &from, const iterator &to)
        {
            beforeRemoveRange(from, to);
            return erase_intern(from, to);
        }

        type extract(const iterator &which)
        {
            beforeRemove(which);
            type temp = std::move(*which);
            erase_intern(which);
            return temp;
        }

        std::pair<iterator, bool> read_item_where(SerializeInStream &in, const const_iterator &where)
        {
            std::pair<iterator, bool> it = read_item_where_intern(in, where);
            if (it.second)
                onInsert(it.first);
            return it;
        }

        void writeState(SerializeOutStream &out, const char *name = nullptr) const
        {
            if (name)
                out.format().beginExtendedCompound(out, name);
            out.write(this->size(), "size");
            if (name)
                out.format().beginCompound(out, name);
            for (const auto &t : *this) {
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
            if (name)
                in.format().beginExtendedCompound(in, name);
            decltype(this->size()) count;
            in.read(count, "size");
            if (name)
                in.format().beginCompound(in, name);
            bool wasActive = beforeReset();
            Base::clear();
            mActiveIterator = Base::begin();
            while (count--) {
                this->read_item_where_intern(in, end(), std::forward<Creator>(creator));
            }
            afterReset(wasActive);
            if (name)
                in.format().endCompound(in, name);
        }

        void applySerializableMap(const std::map<size_t, SerializableUnitBase *> &map)
        {
            for (auto &t : *this) {
                this->applyMap(map, t);
            }
        }

        void setDataSynced(bool b)
        {
            for (auto &t : *this) {
                this->setItemDataSynced(t, b);
            }
        }

        void setActive(bool active)
        {
            if (!active) {
                while (mActiveIterator != begin()) {
                    --mActiveIterator;
                    this->setItemActive(*mActiveIterator, active);
                }
            }
            Serializable<OffsetPtr>::setActive(active);
            if (active) {
                while (mActiveIterator != end()) {
                    type &t = *mActiveIterator;
                    ++mActiveIterator;
                    this->setItemActive(t, active);
                }
            }
        }

    protected:
        void afterInsert(const iterator &it)
        {
            if (this->isSynced()) {
                this->setItemDataSynced(*it, true);
            }
            if (isItemActive(it))
                this->setItemActive(*it, true);
        }

        void beforeRemove(const iterator &it)
        {
            if (this->isSynced()) {
                this->setItemDataSynced(*it, false);
            }
            if (isItemActive(it)) {
                this->setItemActive(*it, false);
            }
        }

        void beforeRemoveRange(const iterator &from, const iterator &to)
        {
            if (this->isSynced()) {
                for (iterator it = from; it != to; ++it) {
                    this->setItemDataSynced(*it, false);
                }
            }
            for (iterator it = from; it != to && isItemActive(it); ++it) {
                this->setItemActive(*it, false);
            }
        }

        bool beforeReset()
        {
            if (this->isSynced()) {
                setDataSynced(false);
            }
            if (this->isActive()) {
                setActive(false);
                return true;
            }
            return false;
        }

        void afterReset(bool wasActive)
        {
            if (this->isSynced()) {
                setDataSynced(true);
            }
            if (wasActive)
                setActive(true);
        }

        template <typename Creator>
        std::pair<iterator, bool> read_item_where_intern(SerializeInStream &in, const const_iterator &where, Creator &&creator)
        {
            if constexpr (UnitHelper<typename container_traits<C>::type>::sIsUnit) {
                in.format().beginExtendedCompound(in, "Item");
            }
            std::pair<iterator, bool> it = emplace_tuple_intern(where, creator.readCreationData(in));
            assert(it.second);
            in.read(*it.first, "Item");
            return it;
        }

        void write_item(SerializeOutStream &out, const type &t) const
        {
            if constexpr (UnitHelper<typename container_traits<C>::type>::sIsUnit) {
                out.format().beginExtendedCompound(out, "Item");
            }
            this->write_creation(out, t);
            out.write(t, "Item");
        }

        bool isItemActive(const iterator &it)
        {
            if (mActiveIterator == end())
                return true;
            for (auto it2 = begin(); it2 != mActiveIterator; ++it2) {
                if (it2 == it)
                    return true;
            }
            return false;
        }

        template <class... _Ty>
        std::pair<iterator, bool> emplace_intern(const const_iterator &where, _Ty &&... args)
        {
            auto keep = _traits::keepIterator(begin(), mActiveIterator);
            std::pair<iterator, bool> it = _traits::emplace(*this, where, std::forward<_Ty>(args)...);
            if (it.second) {
                iterator oldActiveIt = _traits::revalidateIteratorInsert(begin(), keep, it.first);
                if (std::next(it.first) == oldActiveIt && !this->isActive())
                    mActiveIterator = it.first;
                else
                    mActiveIterator = oldActiveIt;
                this->setParent(*it.first, OffsetPtr::parent(this));
            }
            return it;
        }

        template <class... _Ty>
        std::pair<iterator, bool> emplace_tuple_intern(const const_iterator &where, std::tuple<_Ty...> &&tuple)
        {
            return TupleUnpacker::invokeExpand(&SerializableContainerImpl<OffsetPtr, C>::emplace_intern<_Ty...>,
                this,
                where,
                std::forward<std::tuple<_Ty...>>(tuple));
        }

        iterator erase_intern(const iterator &it)
        {
            bool test = mActiveIterator == it;
            auto keep = _traits::keepIterator(begin(), mActiveIterator);
            iterator newIt = Base::erase(it);
            if (test)
                mActiveIterator = newIt;
            else
                mActiveIterator = _traits::revalidateIteratorRemove(begin(), keep, newIt);
            return newIt;
        }

        iterator erase_intern(const iterator &from, const iterator &to)
        {
            bool test = false;
            size_t count = 0;
            for (iterator it = from; it != to; ++it) {
                if (mActiveIterator == it)
                    test = true;
                if (test)
                    ++count;
            }
            auto keep = _traits::keepIterator(begin(), mActiveIterator);
            iterator newIt = Base::erase(from, to);
            if (test)
                mActiveIterator = newIt;
            else
                mActiveIterator = _traits::revalidateIteratorRemove(begin(), keep, newIt, count);
            return newIt;
        }

    protected:
        iterator mActiveIterator;
    };

    template <template <typename...> typename C, typename OffsetPtr>
    using PartialOffsetContainer = partial<C, type_pack<OffsetPtr>>;

    template <typename OffsetPtr, typename... C>
    using SerializableContainer = typename container_traits<C...>::template api<SerializableContainerImpl<OffsetPtr, C...>>;

#define SERIALIZABLE_CONTAINER(Name, ...)                                                                                     \
    ::Engine::Serialize::SerializableContainer<::Engine::Serialize::SerializableOffsetPtr<Self, __LINE__>, __VA_ARGS__> Name; \
    DEFINE_SERIALIZABLE_OFFSET(Name)

#define SERIALIZABLE_CONTAINER_EXT(Name, Pre, Type, ...)                                                                          \
    Pre Serialize::PartialOffsetContainer<Type, Engine::Serialize::SerializableOffsetPtr<Self, __LINE__>>::type __VA_ARGS__ Name; \
    DEFINE_SERIALIZABLE_OFFSET(Name)

}
}
