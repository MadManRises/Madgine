#pragma once

#include "../../keyvalue/observerevent.h"
#include "../streams/bufferedstream.h"
#include "../syncable.h"
#include "serializablecontainer.h"
#include "../../generic/noopfunctor.h"

namespace Engine {
namespace Serialize {

    namespace __syncablecontainer__impl__ {
        enum RequestMode {
            ALL_REQUESTS,
            NO_REQUESTS
        };

        template <RequestMode mode>
        struct ContainerPolicy {
            static constexpr RequestMode requestMode = mode;
        };
    }

    struct ContainerPolicies {
        using allowAll = __syncablecontainer__impl__::ContainerPolicy<__syncablecontainer__impl__::ALL_REQUESTS>;
        using masterOnly = __syncablecontainer__impl__::ContainerPolicy<__syncablecontainer__impl__::NO_REQUESTS>;
    };

    template <typename PtrOffset, typename C, typename Config, typename Observer = NoOpFunctor>
    struct SyncableContainerImpl : SerializableContainerImpl<PtrOffset, C>, Syncable<PtrOffset>, private Observer {

        using _traits = container_traits<C>;

        struct traits : _traits {

            typedef SyncableContainerImpl<PtrOffset, C, Config, Observer> container;

            template <class... _Ty>
            static std::pair<typename _traits::iterator, bool> emplace(container &c, const typename _traits::const_iterator &where, _Ty &&... args)
            {
                return c.emplace(where, std::forward<_Ty>(args)...);
            }
        };

        typedef size_t TransactionId;

        typedef SerializableContainerImpl<PtrOffset, C> Base;

        typedef typename _traits::iterator iterator;
        typedef typename _traits::const_iterator const_iterator;
        typedef typename _traits::type type;
        typedef typename _traits::value_type value_type;

        struct Transaction {
            template <class T>
            Transaction(TransactionId id, T &&callback)
                : mId(id)
                , mCallback(std::forward<T>(callback))
            {
            }

            TransactionId mId;
            std::function<void(const iterator &, bool)> mCallback;
        };

        SyncableContainerImpl() = default;

        SyncableContainerImpl(const SyncableContainerImpl &other)
            : Base(other)
            , mTransactionCounter(0)
        {
        }

        SyncableContainerImpl(SyncableContainerImpl &&other) = default;

        Observer &observer()
        {
            return *this;
        }

        template <class T>
        SyncableContainerImpl<PtrOffset, C, Config, Observer> &operator=(T &&arg)
        {
            if (this->isMaster()) {
                bool wasActive = beforeReset();
                Base::operator=(std::forward<T>(arg));
                afterReset(wasActive, this->end());
            } else {
                if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                    Base temp(std::forward<T>(arg));

                    BufferedOutStream *out = this->getSlaveActionMessageTarget();
                    *out << TransactionId(0);
                    *out << RESET;
                    temp.writeState(*out);
                    out->endMessage();
                } else {
                    throw 0;
                }
            }
            return *this;
        }

        void clear()
        {
            bool wasActive = beforeReset();
            Base::Base::clear();
            this->mActiveIterator = this->begin();
            afterReset(wasActive, this->end());
        }

        template <class... _Ty>
        std::pair<iterator, bool> emplace(const iterator &where, _Ty &&... args)
        {
            std::pair<iterator, bool> it = std::make_pair(this->end(), false);

            if (this->isMaster()) {
                beforeInsert(where);
                it = Base::emplace_intern(where, std::forward<_Ty>(args)...);
                afterInsert(it.second, it.first);
            } else {
                if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                    type temp(std::forward<_Ty>(args)...);

                    BufferedOutStream *out = this->getSlaveActionMessageTarget();
                    *out << TransactionId(0);
                    *out << INSERT_ITEM;
                    this->write_item(*out, where, temp);
                    out->endMessage();
                } else {
                    throw 0;
                }
            }
            return it;
        }

        template <class T, class... _Ty>
        std::pair<iterator, bool> emplace_init(T &&init, const iterator &where, _Ty &&... args)
        {
            std::pair<iterator, bool> it = std::make_pair(this->end(), false);
            if (this->isMaster()) {
                beforeInsert(where);
                it = Base::emplace_intern(where, std::forward<_Ty>(args)...);
                if (it.second) {
                    init(*it.first);
                }
                afterInsert(it.second, it.first);
            } else {
                if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                    type temp(std::forward<_Ty>(args)...);
                    this->postConstruct(temp);

                    init(temp);

                    BufferedOutStream *out = this->getSlaveActionMessageTarget();
                    *out << TransactionId(0);
                    *out << INSERT_ITEM;
                    this->write_item(*out, where, temp);
                    out->endMessage();
                } else {
                    throw 0;
                }
            }
            return it;
        }

        iterator erase(const iterator &where)
        {
            iterator it = this->end();

            if (this->isMaster()) {
                bool b = beforeRemove(where);
                it = Base::erase_intern(where);
                afterRemove(b);
            } else {
                if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                    BufferedOutStream *out = this->getSlaveActionMessageTarget();
                    *out << TransactionId(0);
                    *out << REMOVE_ITEM;
                    this->write_iterator(*out, where);
                    out->endMessage();
                } else {
                    throw 0;
                }
            }
            return it;
        }

        iterator erase(const iterator &from, const iterator &to)
        {
            iterator it = this->end();

            if (this->isMaster()) {
                size_t count = beforeRemoveRange(from, to);
                it = Base::erase_intern(from, to);
                afterRemoveRange(count);
            } else {
                if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                    BufferedOutStream *out = this->getSlaveActionMessageTarget();
                    *out << TransactionId(0);
                    *out << REMOVE_RANGE;
                    this->write_iterator(*out, from);
                    this->write_iterator(*out, to);
                    out->endMessage();
                } else {
                    throw 0;
                }
            }
            return it;
        }

        std::pair<iterator, bool> read_item_where(BufferedInStream &in, const const_iterator &where)
        {
            std::pair<iterator, bool> it = std::make_pair(this->end(), false);
            if (this->isMaster()) {
                beforeInsert(where);
                it = Base::read_item_where_intern(in, where);
                onInsert(it.second, it.first);
            } else {
                throw 0;
            }
            return it;
        }

        template <typename Creator = DefaultCreator<>>
        void readState(SerializeInStream &in, const char *name, Creator &&creator = {}, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        {
            decltype(this->size()) count;
            in >> count;
            bool wasActive = beforeReset();
            Base::Base::clear();
            this->mActiveIterator = Base::Base::begin();
            while (count--) {
                this->read_item_where_intern(in, this->end(), std::forward<Creator>(creator));
            }
            afterReset(wasActive, this->end(), answerTarget, answerId);
        }

        // Inherited via Observable
        template <typename Creator = DefaultCreator<>>
        void readRequest(BufferedInOutStream &inout, Creator &&creator = {})
        {
            bool accepted = (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS); //Check TODO

            TransactionId id;
            inout >> id;

            ObserverEvent op;
            inout >> reinterpret_cast<int &>(op);

            if (!accepted) {
                if (id) {
                    this->beginActionResponseMessage(&inout);
                    inout << id;
                    inout << (op | ABORTED);
                    inout.endMessage();
                }
            } else {
                if (this->isMaster()) {
                    performOperation(op, inout, std::forward<Creator>(creator), inout.id(), id);
                } else {
                    TransactionId newId = 0;
                    if (id) {
                        newId = ++mTransactionCounter;
                    }
                    BufferedOutStream *out = this->getSlaveActionMessageTarget();
                    *out << newId;
                    *out << op;
                    *out << inout;
                    out->endMessage();

                    if (id) {
                        mPassTransactions.emplace_back(newId, std::make_pair(inout.id(), id));
                    }
                }
            }
        }

        template <typename Creator = DefaultCreator<>>
        void readAction(SerializeInStream &inout, Creator &&creator = {})
        {
            TransactionId id;
            inout >> id;

            ObserverEvent op;
            inout >> op;

            bool accepted = id == 0 || (op & ~MASK) == AFTER;

            iterator it = this->end();

            std::pair<ParticipantId, TransactionId> sender = { 0, 0 };
            if (id) {
                if (!mPassTransactions.empty() && mPassTransactions.front().first == id) {
                    sender = mPassTransactions.front().second;
                    mPassTransactions.pop_front();
                }
            }

            if (accepted) {
                it = performOperation(ObserverEvent(op & MASK), inout, std::forward<Creator>(creator), sender.first, sender.second).first;
            }

            if (id) {
                if (!mTransactions.empty()) {
                    assert(mTransactions.front().mId == id);
                    mTransactions.front().mCallback(it, accepted);
                    mTransactions.pop_front();
                }
            }
        }

        template <typename Creator>
        std::pair<iterator, bool> performOperation(ObserverEvent op, SerializeInStream &in, Creator &&creator, ParticipantId partId,
            TransactionId id)
        {
            std::pair<iterator, bool> it = std::make_pair(this->end(), false);
            bool b;
            switch (op) {
            case INSERT_ITEM:
                if constexpr (!_traits::sorted) {
                    it.first = read_iterator(in);
                }
                beforeInsert(it.first);
                it = this->read_item_where_intern(in, it.first, std::forward<Creator>(creator));
                afterInsert(it.second, it.first, partId, id);
                break;
            case REMOVE_ITEM:
                it.first = this->read_iterator(in);
                b = beforeRemove(it.first, partId, id);
                it.first = Base::erase_intern(it.first);
                afterRemove(b);
                it.second = true;
                break;
            case REMOVE_RANGE: {
                iterator from = this->read_iterator(in);
                iterator to = this->read_iterator(in);
                size_t count = beforeRemoveRange(from, to, partId, id);
                it.first = Base::erase_intern(from, to);
                afterRemoveRange(count);
                it.second = true;
                break;
            }
            case RESET:
                readState(in, nullptr, std::forward<Creator>(creator), partId, id);
                it.second = true;
                break;
            default:
                throw 0;
            }
            return it;
        }

        void setActive(bool active)
        {
            if (!active) {
                while (this->mActiveIterator != this->begin()) {
                    --this->mActiveIterator;
                    Observer::operator()(this->mActiveIterator, BEFORE | REMOVE_ITEM);
                    Observer::operator()(this->end(), AFTER | REMOVE_ITEM);
                    this->setItemActive(*this->mActiveIterator, active);
                }
            }
            Serializable<PtrOffset>::setActive(active);
            if (active) {
                while (this->mActiveIterator != this->end()) {
                    auto it = this->mActiveIterator;
                    ++this->mActiveIterator;
                    this->setItemActive(*it, active);
                    Observer::operator()(it, INSERT_ITEM);
                }
            }
        }

    private:
        void beforeInsert(const iterator &it)
        {
            Observer::operator()(it, BEFORE | INSERT_ITEM);
        }

        void afterInsert(bool inserted, const iterator &it, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        {
            if (inserted) {
                if (this->isSynced()) {
                    for (BufferedOutStream *out : this->getMasterActionMessageTargets()) {
                        if (answerTarget == out->id()) {
                            *out << answerId;
                        } else {
                            *out << TransactionId(0);
                        }
                        *out << INSERT_ITEM;
                        this->write_item(*out, it);
                        out->endMessage();
                    }
                    this->setItemDataSynced(*it, true);
                }
                if (this->isItemActive(it)) {
                    this->setItemActive(*it, true);
                }
            }
            if (this->isItemActive(it)) {
                Observer::operator()(it, (inserted ? AFTER : ABORTED) | INSERT_ITEM);
            }
        }

        bool beforeRemove(const iterator &it, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        {
            if (this->isSynced()) {
                for (BufferedOutStream *out : this->getMasterActionMessageTargets()) {
                    if (answerTarget == out->id()) {
                        *out << answerId;
                    } else {
                        *out << TransactionId(0);
                    }
                    *out << REMOVE_ITEM;
                    this->write_iterator(*out, it);
                    out->endMessage();
                }
                this->setItemDataSynced(*it, false);
            }
            if (this->isItemActive(it)) {
                Observer::operator()(it, BEFORE | REMOVE_ITEM);
                this->setItemActive(*it, false);
                return true;
            }
            return false;
        }

        void afterRemove(bool b)
        {
            if (b) {
                Observer::operator()(this->end(), AFTER | REMOVE_ITEM);
            }
        }

        size_t beforeRemoveRange(const iterator &from, const iterator &to, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        {
            if (this->isSynced()) {
                for (BufferedOutStream *out : this->getMasterActionMessageTargets()) {
                    if (answerTarget == out->id()) {
                        *out << answerId;
                    } else {
                        *out << TransactionId(0);
                    }
                    *out << REMOVE_RANGE;
                    this->write_iterator(*out, from);
                    this->write_iterator(*out, to);
                    out->endMessage();
                }
                for (iterator it = from; it != to; ++it) {
                    this->setItemDataSynced(*it, false);
                }
            }

            size_t count = 0;
            for (iterator it = from; it != to && this->isItemActive(it); ++it) {
                Observer::operator()(it, BEFORE | REMOVE_ITEM);
                this->setItemActive(*it, false);
                ++count;
            }
            return count;
        }

        void afterRemoveRange(size_t count)
        {
            for (size_t i = 0; i < count; ++i) {
                Observer::operator()(this->end(), AFTER | REMOVE_ITEM);
            }
        }

        bool beforeReset()
        {
            if (this->isActive()) {
                Observer::operator()(this->end(), BEFORE | RESET);
            }
            return Base::beforeReset();
        }

        void afterReset(bool wasActive, const iterator &it, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        {
            if (this->isSynced()) {
                for (BufferedOutStream *out : this->getMasterActionMessageTargets()) {
                    if (answerTarget == out->id()) {
                        *out << answerId;
                    } else {
                        *out << TransactionId(0);
                    }
                    *out << RESET;
                    this->writeState(*out);
                    out->endMessage();
                }
            }
            Base::afterReset(wasActive);
            if (wasActive) {
                Observer::operator()(it, AFTER | RESET);
            }
        }

        iterator read_iterator(SerializeInStream &in)
        {
            if constexpr (_traits::sorted) {
                typename _traits::key_type key;
                in >> key;
                return kvFind(*this, key);
            } else {
                int i;
                in >> i;
                return std::next(this->begin(), i);
            }
        }

        void write_iterator(SerializeOutStream &out, const const_iterator &it) const
        {
            if constexpr (_traits::sorted) {
                out << kvKey(*it);
            } else {
                out << static_cast<int>(std::distance(this->begin(), it));
            }
        }

        template <typename Creator>
        std::pair<iterator, bool> read_item(SerializeInStream &in, Creator &&creator)
        {
            iterator it = this->end();
            if constexpr (!_traits::sorted) {
                it = read_iterator(in);
            }
            return this->read_item_where_intern(in, it, std::forward<Creator>(creator));
        }

        void write_item(SerializeOutStream &out, const const_iterator &it) const
        {
            this->write_item(out, it, *it);
        }

        void write_item(SerializeOutStream &out, const const_iterator &it, const type &t) const
        {
            if constexpr (!_traits::sorted) {
                write_iterator(out, it);
            }
            this->write_item(out, t);
        }

        using Base::write_item;

    private:
        std::list<Transaction> mTransactions;
        std::list<std::pair<TransactionId, std::pair<ParticipantId, TransactionId>>> mPassTransactions;

        TransactionId mTransactionCounter = 0;
    };

	template <typename OffsetPtr, typename C, typename Config, typename Observer = NoOpFunctor>
    using SyncableContainer = typename container_traits<C>::template api<SyncableContainerImpl<OffsetPtr, C, Config, Observer>>;

	#define SYNCABLE_CONTAINER(Name, ...)                                                                                     \
    ::Engine::Serialize::SyncableContainer<::Engine::Serialize::CombinedOffsetPtr<Self, __LINE__>, __VA_ARGS__> Name; \
    DEFINE_COMBINED_OFFSET(Name)

}
}
