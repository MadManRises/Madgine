#pragma once

#include "../../generic/noopfunctor.h"
#include "../../generic/observerevent.h"
//#include "../../keyvalue/keyvalue.h"
#include "../../generic/future.h"
#include "../../generic/onetimefunctor.h"
#include "../streams/bufferedstream.h"
#include "../streams/operations.h"
#include "../streams/pendingrequest.h"
#include "../syncable.h"
#include "requestbuilder.h"
#include "serializablecontainer.h"
#include "../../generic/container/container_api.h"

namespace Engine {
namespace Serialize {

    template <typename Builder>
    struct EmplaceRequestBuilder;

    template <typename Builder>
    struct EmplaceRequestBuilder : RequestBuilder<Builder> {

        template <typename C>
        auto init(C &&c)
        {
            return this->template append<3>(std::forward<C>(c));
        }
    };

    template <typename F>
    EmplaceRequestBuilder(F &&f)->EmplaceRequestBuilder<Builder<F, EmplaceRequestBuilder, 4>>;

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

    template <typename C, typename Config, typename Observer = NoOpFunctor, typename controlled = std::false_type, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
    struct SyncableContainerImpl : SerializableContainerImpl<C, Observer, controlled, OffsetPtr>, Syncable<OffsetPtr> {

        using _traits = container_traits<C>;
        using container_t = SyncableContainerImpl<C, Config, Observer, controlled, OffsetPtr>;
        
        typedef SerializableContainerImpl<C, Observer, controlled, OffsetPtr> Base;

        typedef typename _traits::iterator iterator;
        typedef typename _traits::const_iterator const_iterator;
        typedef typename _traits::value_type value_type;

        SyncableContainerImpl() = default;

        SyncableContainerImpl(const SyncableContainerImpl &other) = default;

        SyncableContainerImpl(SyncableContainerImpl &&other) = default;

        template <typename T>
        SyncableContainerImpl<C, Config, Observer, controlled, OffsetPtr> &operator=(T &&arg)
        {
            if (this->isMaster()) {
                ResetOperation { *this } = std::forward<T>(arg);
            } else {
                if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                    Base temp(std::forward<T>(arg));

                    BufferedOutStream *out = this->getSlaveActionMessageTarget();
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
            ResetOperation { *this }.clear();
        }

        template <typename... _Ty>
        auto emplace(const iterator &where, _Ty &&... args)
        {
            return EmplaceRequestBuilder {
                [=, args = std::tuple<_Ty...> { std::forward<_Ty>(args)... }](auto &&then, auto &&onSuccess, auto &&onFailure, auto &&init) mutable -> Future<typename _traits::emplace_return> {
                                                                                                                                                        if (this->isMaster()) {
                                                                                                                                                            typename _traits::emplace_return it = TupleUnpacker::invokeExpand(LIFT_MEMBER(emplace_impl), this, where, std::forward<decltype(init)>(init), std::move(args));
            executeCallbacks(it, std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(then)>(then));
            return it;
        }
        else
        {
            if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                std::promise<typename _traits::emplace_return> p;
                Future<typename _traits::emplace_return> future = p.get_future();

                BufferedOutStream *out = this->getSlaveActionMessageTarget(0, 0, generateCallback(std::move(p), std::forward<decltype(then)>(then), std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(onFailure)>(onFailure)));
                *out << INSERT_ITEM;
                value_type temp = TupleUnpacker::constructFromTuple<value_type>(std::move(args));
                TupleUnpacker::forEach(std::forward<decltype(init)>(init), [&](auto &&f) { TupleUnpacker::invoke(f, temp); });
                //write_item(*out, where, temp);
                throw "TODO";
                out->endMessage();

                return future;
            } else {
                std::terminate();
            }
        }
    }
};
}

/*template <typename T, typename... _Ty>
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
                    *out << INSERT_ITEM;
                    write_item(*out, where, temp);
                    out->endMessage();
                } else {
                    std::terminate();
                }
            }
            return it;
        }*/

auto erase(const iterator &where)
{
    return RequestBuilder {
        [=](auto &&then, auto &&onSuccess, auto &&onFailure) mutable -> Future<iterator> {
            if (this->isMaster()) {
                iterator it = erase_impl(where);
                executeCallbacks(it, std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(then)>(then));
                return it;
            } else {
                if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                    std::promise<iterator> p;
                    Future<iterator> future = p.get_future();

                    BufferedOutStream *out = this->getSlaveActionMessageTarget(0, 0, generateCallback(std::move(p), std::forward<decltype(then)>(then), std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(onFailure)>(onFailure)));
                    *out << REMOVE_ITEM;
                    this->write_iterator(*out, where);
                    out->endMessage();

                    return future;
                } else {
                    std::terminate();
                }
            }
        }
    };
}

auto erase(const iterator &from, const iterator &to)
{
    iterator it = this->end();
    return RequestBuilder {
        [=](auto &&then, auto &&onSuccess, auto &&onFailure) mutable -> Future<iterator> {
            if (this->isMaster()) {
                iterator it = erase_impl(from, to);
                executeCallbacks(it, std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(then)>(then));
                return it;
            } else {
                if constexpr (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS) {
                    std::promise<iterator> p;
                    Future<iterator> future = p.get_future();

                    BufferedOutStream *out = this->getSlaveActionMessageTarget(0, 0, generateCallback(std::move(p), std::forward<decltype(then)>(then), std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(onFailure)>(onFailure)));
                    *out << REMOVE_RANGE;
                    this->write_iterator(*out, from);
                    this->write_iterator(*out, to);
                    out->endMessage();

                    return future;
                } else {
                    std::terminate();
                }
            }
        }
    };
}

/*std::pair<iterator, bool> read_item_where(BufferedInStream & in, const const_iterator &where)
    {
        std::pair<iterator, bool> it = std::make_pair(this->end(), false);
        if (this->isMaster()) {
            it = read_item_where_impl(in, where);
        } else {
            std::terminate();
        }
        return it;
    }*/

/*void readState(SerializeInStream &in, const char *name)
{
    readState_impl(in, name);
}*/

void readRequest(BufferedInOutStream &inout, TransactionId id)
{
    bool accepted = (Config::requestMode == __syncablecontainer__impl__::ALL_REQUESTS); //Check TODO

    ObserverEvent op;
    inout >> reinterpret_cast<int &>(op);

    if (!accepted) {
        if (id) {
            this->beginActionResponseMessage(&inout, id);
            inout << (op | ABORTED);
            inout.endMessage();
        }
    } else {
        if (this->isMaster()) {
            performOperation(op, inout, inout.id(), id);
        } else {
            BufferedOutStream *out = this->getSlaveActionMessageTarget(inout.id(), id);
            *out << op;
            out->pipe(inout);
            out->endMessage();
        }
    }
}

void readAction(SerializeInStream &inout, PendingRequest *request)
{
    ObserverEvent op;
    inout >> op;

    bool accepted = (op & ~MASK) != ABORTED;

    if (accepted) {
        typename _traits::emplace_return it = performOperation(ObserverEvent(op & MASK), inout, request ? request->mRequester : 0, request ? request->mRequesterTransactionId : 0);
        if (request && request->mCallback)
            request->mCallback(&it);
    } else {
        if (request) {
            if (request->mRequesterTransactionId) {
                BufferedOutStream *out = this->beginActionResponseMessage(request->mRequester, request->mRequesterTransactionId);
                *out << op;
                out->endMessage();
            }
            if (request->mCallback)
                request->mCallback(nullptr);
        }
    }
}

typename _traits::emplace_return performOperation(ObserverEvent op, SerializeInStream &in, ParticipantId answerTarget, TransactionId answerId)
{
    typename _traits::emplace_return it = this->end();
    switch (op) {
    case INSERT_ITEM:
        if constexpr (!_traits::sorted) {
            it = read_iterator(in);
        }
        //it = read_item_where_impl(in, it, answerTarget, answerId);
        throw "TODO";
        break;
    case REMOVE_ITEM:
        it = erase_impl(this->read_iterator(in), answerTarget, answerId);
        break;
    case REMOVE_RANGE: {
        iterator from = this->read_iterator(in);
        iterator to = this->read_iterator(in);
        it = erase_impl(from, to, answerTarget, answerId);
        break;
    }
    case RESET:
        //readState_impl(in, nullptr, answerTarget, answerId);
        throw "TODO";
        break;
    default:
        std::terminate();
    }
    return it;
}

struct InsertOperation : Base::InsertOperation {
    InsertOperation(container_t &c, const iterator &where, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        : Base::InsertOperation(c, where)
        , mAnswerTarget(answerTarget)
        , mAnswerId(answerId)
    {
    }
    ~InsertOperation()
    {
        assert(this->mTriggered == 1);
        if (this->mLastInserted) {
            if (this->mContainer.isSynced()) {
                for (BufferedOutStream *out : container().getMasterActionMessageTargets(mAnswerTarget, mAnswerId)) {
                    *out << INSERT_ITEM;
                    //container().write_item(*out, this->mLastIt, *this->mLastIt);
                    throw "TODO";
                    out->endMessage();
                }
            }
        }
    }

    container_t &container()
    {
        return static_cast<container_t &>(this->mContainer);
    }

private:
    ParticipantId mAnswerTarget;
    TransactionId mAnswerId;
};

struct RemoveOperation : Base::RemoveOperation {
    RemoveOperation(container_t &c, const iterator &it, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        : Base::RemoveOperation(c, it)
    {
        if (this->mContainer.isSynced()) {
            for (BufferedOutStream *out : container().getMasterActionMessageTargets(answerTarget, answerId)) {
                *out << REMOVE_ITEM;
                //container().write_iterator(*out, it);
                throw "TODO";
                out->endMessage();
            }
        }
    }

    container_t &container()
    {
        return static_cast<container_t &>(this->mContainer);
    }
};

struct RemoveRangeOperation : Base::RemoveRangeOperation {
    RemoveRangeOperation(container_t &c, const iterator &from, const iterator &to, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        : Base::RemoveRangeOperation(c, from, to)
    {
        if (this->mContainer.isSynced()) {
            for (BufferedOutStream *out : container().getMasterActionMessageTargets(answerTarget, answerId)) {
                *out << REMOVE_RANGE;
                /*container().write_iterator(*out, from);
                container().write_iterator(*out, to);*/
                throw "TODO";
                out->endMessage();
            }
        }
    }

    container_t &container()
    {
        return static_cast<container_t &>(this->mContainer);
    }
};

struct ResetOperation : Base::ResetOperation {
    ResetOperation(container_t &c, ParticipantId answerTarget = 0, TransactionId answerId = 0)
        : Base::ResetOperation(c)
        , mAnswerTarget(answerTarget)
        , mAnswerId(answerId)
    {
    }
    ~ResetOperation()
    {
        if (this->mContainer.isSynced()) {
            for (BufferedOutStream *out : container().getMasterActionMessageTargets(mAnswerTarget, mAnswerId)) {
                *out << RESET;
                write(*out, this->mContainer, nullptr);
                out->endMessage();
            }
        }
    }

    container_t &container()
    {
        return static_cast<container_t &>(this->mContainer);
    }

private:
    ParticipantId mAnswerTarget;
    TransactionId mAnswerId;
};

private:
template <typename T, typename... Callbacks>
static void executeCallbacks(const T &t, Callbacks &&... callbacks)
{
    (TupleUnpacker::forEach(std::forward<Callbacks>(callbacks), [&](auto &&f) { TupleUnpacker::invoke(f, t); }), ...);
}

template <typename T, typename Then, typename OnSuccess, typename OnFailure>
std::function<void(void *)> generateCallback(std::promise<T> p, Then &&then, OnSuccess &&onSuccess, OnFailure &&onFailure)
{
    return oneTimeFunctor([p = std::move(p), then = std::forward<Then>(then), onSuccess = std::forward<OnSuccess>(onSuccess), onFailure = std::forward<OnFailure>(onFailure)](void *data) mutable {
        if (data) {
            T *t = static_cast<T *>(data);
            executeCallbacks(*t, std::move(onSuccess), std::move(then));
            p.set_value(std::move(*t));
        } else {
            TupleUnpacker::forEach(std::move(onFailure), [&](auto &&f) { std::forward<decltype(f)>(f)(); });
            TupleUnpacker::forEach(std::move(then), [&](auto &&f) { TupleUnpacker::invoke(std::forward<decltype(f)>(f), std::nullopt); });
        }
    });
}

iterator read_iterator(SerializeInStream &in)
{
    /*if constexpr (_traits::sorted) {
                FixString_t<KeyType_t<typename _traits::value_type>> key;
                in >> key;
                return kvFind(*this, key);
            } else*/
    {
        int i;
        in >> i;
        return std::next(Base::Base::begin(), i);
    }
}

/*void write_iterator(SerializeOutStream &out, const const_iterator &it) const
{
    if constexpr (_traits::sorted) {
                out << kvKey(*it);
            } else
    {
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
    return Base::read_item_where_intern(in, it, std::forward<Creator>(creator));
}

void write_item(SerializeOutStream &out, const const_iterator &it, const value_type &t) const
{
    if constexpr (!_traits::sorted) {
        write_iterator(out, it);
    }
    Base::write_item(out, t);
}*/

template <typename Init, typename... _Ty>
typename _traits::emplace_return emplace_impl(const iterator &where, Init &&init, _Ty &&... args)
{
    InsertOperation op { *this, where };
    typename _traits::emplace_return it = op.emplace(where, std::forward<_Ty>(args)...);
    TupleUnpacker::forEach(std::forward<Init>(init), [&](auto &&f) { TupleUnpacker::invoke(f, *it); });
    return it;
}

iterator erase_impl(const iterator &it, ParticipantId answerTarget = 0, TransactionId answerId = 0)
{
    return RemoveOperation { *this, it, answerTarget, answerId }.erase(it);
}

iterator erase_impl(const iterator &from, const iterator &to, ParticipantId answerTarget = 0, TransactionId answerId = 0)
{
    return RemoveRangeOperation { *this, from, to, answerTarget, answerId }.erase(from, to);
}

/*void readState_impl(SerializeInStream &in, const char *name, ParticipantId answerTarget = 0, TransactionId answerId = 0)
{
    ResetOperation op { *this, answerTarget, answerId };
    readContainerOp<container, typename Base::Config>(in, *this, name, op, OffsetPtr::parent(this));
}

typename _traits::emplace_return read_item_where_impl(SerializeInStream &in, const iterator &where, ParticipantId answerTarget = 0, TransactionId answerId = 0)
{
    InsertOperation op { *this, where, answerTarget, answerId };
    return Base::read_item_where_intern(in, where, op);
}*/
}
;

template <typename C, typename Config, typename Observer = NoOpFunctor, typename controlled = std::false_type, typename OffsetPtr = TaggedPlaceholder<OffsetPtrTag, 0>>
using SyncableContainer = container_api<SyncableContainerImpl<C, Config, Observer, controlled, OffsetPtr>>;

#define SYNCABLE_CONTAINER(Name, ...) OFFSET_CONTAINER(Name, ::Engine::Serialize::SyncableContainer<__VA_ARGS__>)

}


template <typename C, typename Config, typename Observer, typename controlled, typename _OffsetPtr>
struct underlying_container<Serialize::SyncableContainerImpl<C, Config, Observer, controlled, _OffsetPtr>> {
    typedef C type;
};

template <typename C, typename Config, typename Observer, typename controlled, typename _OffsetPtr>
struct container_traits<Serialize::SyncableContainerImpl<C, Config, Observer, controlled, _OffsetPtr>> : container_traits<C> {
    typedef Serialize::SyncableContainerImpl<C, Config, Observer, controlled, _OffsetPtr> container;

    using _traits = container_traits<C>;

    template <typename... Args>
    static typename _traits::emplace_return emplace(container& c, const typename _traits::iterator& where, Args&&... args) {
        return c.emplace(where, std::forward<Args>(args)...);
    }
};



}
