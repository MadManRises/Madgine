#pragma once

#include "../../keyvalue/observablecontainer.h" //TODO
#include "../../signalslot/connection.h"
#include "../../signalslot/signal.h"
#include "../syncable.h"
#include "../streams/bufferedstream.h"
#include "operations.h"
#include "serializablecontainer.h"
#include "sortedcontainer.h"
#include "unsortedcontainer.h"

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

        template <typename PtrOffset, class traits>
        using ContainerSelector = std::conditional_t<
            traits::sorted,
            SortedContainer<PtrOffset, traits>,
            UnsortedContainer<PtrOffset, traits>>;
    }

    struct ContainerPolicies {
        using allowAll = __syncablecontainer__impl__::ContainerPolicy<__syncablecontainer__impl__::ALL_REQUESTS>;
        using masterOnly = __syncablecontainer__impl__::ContainerPolicy<__syncablecontainer__impl__::NO_REQUESTS>;
    };

    template <typename PtrOffset, class traits, typename Config>
    class SyncableContainer : public __syncablecontainer__impl__::ContainerSelector<PtrOffset, traits>, public Syncable<PtrOffset> {
    public:
        typedef size_t TransactionId;

        typedef __syncablecontainer__impl__::ContainerSelector<PtrOffset, traits> Base;

        typedef typename traits::iterator iterator;
        typedef typename traits::const_iterator const_iterator;
        typedef typename traits::type type;
        typedef typename traits::value_type value_type;

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

        SyncableContainer()
            : mTransactionCounter(0)
        {
        }

        SyncableContainer(const SyncableContainer &other)
            : Base(other)
            , mTransactionCounter(0)
        {
        }

        SyncableContainer(SyncableContainer &&other) = default;

        ~SyncableContainer()
        {
        }

        template <class T>
        void operator=(T &&arg)
        {
            if (this->isMaster()) {
                bool wasActive = beforeReset(this->end());
                Base::operator=(std::forward<T>(arg));
                afterReset(wasActive, this->end());
            } else {
                if constexpr (Config::requestMode == __observablecontainer__impl__::ALL_REQUESTS) {
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
        }

        void clear()
        {
            bool wasActive = beforeReset(this->end());
            this->mData.clear();
            this->mActiveIterator = this->mData.begin();
            afterReset(wasActive, this->end());
        }

        template <class... _Ty>
        std::pair<iterator, bool> emplace(const iterator &where, _Ty &&... args)
        {
            std::pair<iterator, bool> it = std::make_pair(this->end(), false);

            if (this->isMaster()) {
                it = Base::emplace_intern(where, std::forward<_Ty>(args)...);
                if (it.second)
                    onInsert(it.first);
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
                it = Base::emplace_intern(where, std::forward<_Ty>(args)...);
                if (it.second) {
                    init(*it.first);
                    onInsert(it.first);
                }
            } else {
                if constexpr (Config::requestMode == __observablecontainer__impl__::ALL_REQUESTS) {
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

        std::pair<iterator, bool> read_item_where(const const_iterator &where, BufferedInStream &in)
        {
            std::pair<iterator, bool> it = std::make_pair(this->end(), false);
            if (this->isMaster()) {
                it = Base::read_item_where_intern(where, in);
                if (it.second)
                    onInsert(it.first);
            } else {
                throw 0;
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
                if constexpr (Config::requestMode == __observablecontainer__impl__::ALL_REQUESTS) {
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

        template <typename Creator>
        std::pair<iterator, bool> performOperation(Operations op, SerializeInStream &in, Creator &&creator, ParticipantId partId,
            TransactionId id)
        {
            std::pair<iterator, bool> it = std::make_pair(this->end(), false);
            bool b;
            switch (op) {
            case INSERT_ITEM:
                it = this->read_item(in, std::forward<Creator>(creator));
                if (it.second)
                    onInsert(it.first, partId, id);
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
                readState(in, std::forward<Creator>(creator), partId, id);
                it.second = true;
                break;
            default:
                throw 0;
            }
            return it;
        }

        template <typename Creator = DefaultCreator<>>
        void readState(SerializeInStream &in, Creator &&creator = {}, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        {
            bool wasActive = beforeReset(this->end());
            this->mData.clear();
            this->mActiveIterator = this->mData.begin();
            while (in.loopRead()) {
                this->read_item_where_intern(this->end(), in, std::forward<Creator>(creator));
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

            Operations op;
            inout >> reinterpret_cast<int &>(op);

            if (!accepted) {
                if (id) {
                    this->beginActionResponseMessage(&inout);
                    inout << id;
                    inout << (op | REJECT);
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

            Operations op;
            inout >> op;

            bool accepted = id == 0 || (op & ~MASK) == ACCEPT;

            iterator it = this->end();

            std::pair<ParticipantId, TransactionId> sender = { 0, 0 };
            if (id) {
                if (!mPassTransactions.empty() && mPassTransactions.front().first == id) {
                    sender = mPassTransactions.front().second;
                    mPassTransactions.pop_front();
                }
            }

            if (accepted) {
                it = performOperation(Operations(op & MASK), inout, std::forward<Creator>(creator), sender.first, sender.second).first;
            }

            if (id) {
                if (!mTransactions.empty()) {
                    assert(mTransactions.front().mId == id);
                    mTransactions.front().mCallback(it, accepted);
                    mTransactions.pop_front();
                }
            }
        }

        SignalSlot::SignalStub<const iterator &, int> &signal()
        {
            return mSignal;
        }

    protected:
        void setActive(bool active)
        {
            if (!active) {
                while (this->mActiveIterator != this->begin()) {
                    --this->mActiveIterator;
                    mSignal.emit(this->mActiveIterator, BEFORE | REMOVE_ITEM);
                    mSignal.emit(this->end(), AFTER | REMOVE_ITEM);
                    this->setItemActive(*this->mActiveIterator, active);
                }
            }
            _Serializable<PtrOffset>::setActive(active);
            if (active) {
                while (this->mActiveIterator != this->end()) {
                    auto it = this->mActiveIterator;
                    ++this->mActiveIterator;
                    this->setItemActive(*it, active);
                    mSignal.emit(it, INSERT_ITEM);
                }
            }
        }

    private:
        void onInsert(const iterator &it, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        {
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
                mSignal.emit(it, INSERT_ITEM);
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
                mSignal.emit(it, BEFORE | REMOVE_ITEM);
                this->setItemActive(*it, false);
                return true;
            }
            return false;
        }

        void afterRemove(bool b)
        {
            if (b) {
                mSignal.emit(this->end(), AFTER | REMOVE_ITEM);
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
                mSignal.emit(it, BEFORE | REMOVE_ITEM);
                this->setItemActive(*it, false);
                ++count;
            }
            return count;
        }

        void afterRemoveRange(size_t count)
        {
            for (size_t i = 0; i < count; ++i) {
                mSignal.emit(this->end(), AFTER | REMOVE_ITEM);
            }
        }

        bool beforeReset(const iterator &it)
        {
            if (this->isActive()) {
                mSignal.emit(it, BEFORE | RESET);
            }
            return this->unsync();
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
            this->sync(wasActive);
            if (wasActive) {
                mSignal.emit(it, AFTER | RESET);
            }
        }

    private:
        SignalSlot::Signal<const iterator &, int> mSignal;

        std::list<Transaction> mTransactions;
        std::list<std::pair<TransactionId, std::pair<ParticipantId, TransactionId>>> mPassTransactions;

        TransactionId mTransactionCounter;
    };
}
}
