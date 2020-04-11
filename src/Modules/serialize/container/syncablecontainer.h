#pragma once

#include "../../generic/noopfunctor.h"
#include "../../generic/observerevent.h"
//#include "../../keyvalue/keyvalue.h"
#include "../streams/bufferedstream.h"
#include "../syncable.h"
#include "serializablecontainer.h"

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

    template <typename C, typename Config, typename Observer = NoOpFunctor, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    struct SyncableContainerImpl : SerializableContainerImpl<C, Observer, OffsetPtr>, Syncable<OffsetPtr> {

        using _traits = container_traits<C>;

        struct traits : _traits {

            typedef SyncableContainerImpl<C, Config, Observer, OffsetPtr> container;

            template <typename... _Ty>
            static std::pair<typename _traits::iterator, bool> emplace(container &c, const typename _traits::const_iterator &where, _Ty &&... args)
            {
                return c.emplace(where, std::forward<_Ty>(args)...);
            }
        };

        typedef size_t TransactionId;

        typedef SerializableContainerImpl<C, Observer, OffsetPtr> Base;

        typedef typename _traits::iterator iterator;
        typedef typename _traits::const_iterator const_iterator;
        typedef typename _traits::value_type value_type;

        struct Transaction {
            template <typename T>
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

        template <typename T>
        SyncableContainerImpl<C, Config, Observer, OffsetPtr> &operator=(T &&arg)
        {
            if (this->isMaster()) {
                bool wasActive = beforeReset();
                Base::operator=(std::forward<T>(arg));
                afterReset(wasActive);
            } else {
                if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                    Base temp(std::forward<T>(arg));

                    BufferedOutStream *out = this->getSlaveActionMessageTarget();
                    *out << TransactionId(0);
                    *out << RESET;
                    temp.writeState(*out);
                    out->endMessage();
                } else {
                    std::terminate();
                }
            }
            return *this;
        }

        void clear()
        {
            bool wasActive = beforeReset();
            Base::Base::clear();
            this->mActiveIterator = Base::Base::begin();
            afterReset(wasActive);
        }

        template <typename... _Ty>
        std::pair<iterator, bool> emplace(const iterator &where, _Ty &&... args)
        {
            std::pair<iterator, bool> it = std::make_pair(this->end(), false);

            if (this->isMaster()) {
                beforeInsert(where);
                it = Base::emplace_intern(where, std::forward<_Ty>(args)...);
                afterInsert(it.second, it.first);
            } else {
                if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                    value_type temp(std::forward<_Ty>(args)...);

                    BufferedOutStream *out = this->getSlaveActionMessageTarget();
                    *out << TransactionId(0);
                    *out << INSERT_ITEM;
                    write_item(*out, where, temp);
                    out->endMessage();
                } else {
                    std::terminate();
                }
            }
            return it;
        }

        template <typename T, typename... _Ty>
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
                    value_type temp(std::forward<_Ty>(args)...);
                    this->postConstruct(temp);

                    init(temp);

                    BufferedOutStream *out = this->getSlaveActionMessageTarget();
                    *out << TransactionId(0);
                    *out << INSERT_ITEM;
                    write_item(*out, where, temp);
                    out->endMessage();
                } else {
                    std::terminate();
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
                afterRemove(b, it);
            } else {
                if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                    BufferedOutStream *out = this->getSlaveActionMessageTarget();
                    *out << TransactionId(0);
                    *out << REMOVE_ITEM;
                    this->write_iterator(*out, where);
                    out->endMessage();
                } else {
                    std::terminate();
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
                    std::terminate();
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
                std::terminate();
            }
            return it;
        }

        template <typename Creator = DefaultCreator<>>
        void readState(SerializeInStream &in, const char *name, Creator &&creator = {})
        {
            bool wasActive = beforeReset();
            Base::readState_intern(in, name, std::forward<Creator>(creator));
            afterReset(wasActive);
        }

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
                    out->pipe(inout);
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
            switch (op) {
            case INSERT_ITEM:
                if constexpr (!_traits::sorted) {
                    it.first = read_iterator(in);
                }
                beforeInsert(it.first);
                it = this->read_item_where_intern(in, it.first, std::forward<Creator>(creator));
                afterInsert(it.second, it.first, partId, id);
                break;
            case REMOVE_ITEM: {
                it.first = this->read_iterator(in);
                bool b = beforeRemove(it.first, partId, id);
                it.first = Base::erase_intern(it.first);
                afterRemove(b, it.first);
                it.second = true;
                break;
            }
            case REMOVE_RANGE: {
                iterator from = this->read_iterator(in);
                iterator to = this->read_iterator(in);
                size_t count = beforeRemoveRange(from, to, partId, id);
                it.first = Base::erase_intern(from, to);
                afterRemoveRange(count, it.first);
                it.second = true;
                break;
            }
            case RESET: {
                bool wasActive = beforeReset();
                Base::readState_intern(in, nullptr, std::forward<Creator>(creator));
                afterReset(wasActive, partId, id);
                it.second = true;
                break;
            }
            default:
                std::terminate();
            }
            return it;
        }

    private:
        void beforeInsert(const iterator &it)
        {
            Base::beforeInsert(it);
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
                        write_item(*out, it, *it);
                        out->endMessage();
                    }
                }
            }
            Base::afterInsert(inserted, it);
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
            }
            return Base::beforeRemove(it);
        }

        void afterRemove(bool b, const iterator &it)
        {
            Base::afterRemove(b, it);
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
            }

            return Base::beforeRemoveRange(from, to);
        }

        void afterRemoveRange(size_t count, const iterator &it)
        {
            Base::afterRemoveRange(count, it);
        }

        bool beforeReset()
        {
            return Base::beforeReset();
        }

        void afterReset(bool wasActive, ParticipantId answerTarget = 0, TransactionId answerId = 0)
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
        }

        iterator read_iterator(SerializeInStream &in)
        {
            /*if constexpr (_traits::sorted) {
                FixString_t<KeyType_t<typename _traits::value_type>> key;
                in >> key;
                return kvFind(*this, key);
            } else*/ {
                int i;
                in >> i;
                return std::next(Base::Base::begin(), i);
            }
        }

        void write_iterator(SerializeOutStream &out, const const_iterator &it) const
        {
            /*if constexpr (_traits::sorted) {
                out << kvKey(*it);
            } else*/ {
                out << static_cast<int>(std::distance(Base::Base::begin(), it));
            }
        }

        template <typename Creator>
        std::pair<iterator, bool> read_item(SerializeInStream &in, Creator &&creator)
        {
            iterator it;
            if constexpr (_traits::sorted) {
                it = this->end();
            } else {
                it = read_iterator(in);
            }
            return this->read_item_where_intern(in, it, std::forward<Creator>(creator));
        }

        void write_item(SerializeOutStream &out, const const_iterator &it, const value_type &t) const
        {
            if constexpr (!_traits::sorted) {
                write_iterator(out, it);
            }
            Base::write_item(out, t);
        }

    private:
        std::list<Transaction> mTransactions;
        std::list<std::pair<TransactionId, std::pair<ParticipantId, TransactionId>>> mPassTransactions;

        TransactionId mTransactionCounter = 0;
    };

    template <typename C, typename Config, typename Observer = NoOpFunctor, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    using SyncableContainer = typename container_traits<C>::template api<SyncableContainerImpl<C, Config, Observer, OffsetPtr>>;

#define SYNCABLE_CONTAINER(Name, ...) OFFSET_CONTAINER(Name, ::Engine::Serialize::SyncableContainer<__VA_ARGS__>)

}
}
