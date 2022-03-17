#pragma once

#include "requestbuilder.h"

#include "serializablecontainer.h"
#include "syncable.h"
#include "Generic/future.h"

#include "Generic/container/containerevent.h"

namespace Engine {
namespace Serialize {

    template <typename Builder>
    struct EmplaceRequestBuilder;

    template <typename Builder>
    struct EmplaceRequestBuilder : RequestBuilder<Builder> {

        using RequestBuilder<Builder>::RequestBuilder;

        template <typename C>
        auto init(C &&c) &&
        {
            return std::move(*this).template append<3>(std::forward<C>(c));
        }
    };

    template <typename F>
    EmplaceRequestBuilder(F &&f) -> EmplaceRequestBuilder<Builder<F, EmplaceRequestBuilder, 4>>;

    template <typename C, typename Observer = NoOpFunctor, typename OffsetPtr = TaggedPlaceholder<MemberOffsetPtrTag, 0>>
    struct SyncableContainerImpl : SerializableContainerImpl<C, Observer, OffsetPtr>, Syncable<OffsetPtr> {

        using _traits = container_traits<C>;
        using container_t = SyncableContainerImpl<C, Observer, OffsetPtr>;

        typedef SerializableContainerImpl<C, Observer, OffsetPtr> Base;

        typedef typename _traits::iterator iterator;
        typedef typename _traits::const_iterator const_iterator;
        typedef typename _traits::value_type value_type;

        SyncableContainerImpl() = default;

        SyncableContainerImpl(const SyncableContainerImpl &other) = default;

        SyncableContainerImpl(SyncableContainerImpl &&other) = default;

        SyncableContainerImpl<C, Observer, OffsetPtr> &operator=(const typename Base::Base &other)
        {
            if (this->isMaster()) {
                ResetOperation { *this, false } = other;
            } else {
                std::pair<ContainerEvent, typename Base::Base> temp { RESET, other };

                this->writeRequest(&temp); //??? Is the temp object used?
            }
            return *this;
        }

        SyncableContainerImpl<C, Observer, OffsetPtr> &operator=(SyncableContainerImpl<C, Observer, OffsetPtr> &&other)
        {
            Base::operator=(std::move(other));
            return *this;
        }

        void clear()
        {
            if (this->isMaster()) {
                ResetOperation { *this, false }.clear();
            } else {
                ContainerEvent op = RESET;
                this->writeRequest(&op);
            }
        }

        template <typename... _Ty>
        auto emplace(const iterator &where, _Ty &&... args)
        {
            return EmplaceRequestBuilder {
                [=, args = std::tuple<_Ty...> { std::forward<_Ty>(args)... }](auto &&onResult, auto &&onSuccess, auto &&onFailure, auto &&init) mutable -> Future<typename _traits::emplace_return> {
                    if (this->isMaster()) {
                        typename _traits::emplace_return it = TupleUnpacker::invokeExpand(LIFT(emplace_impl, this), where, std::forward<decltype(init)>(init), std::move(args));
                        executeCallbacks(it, std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(onResult)>(onResult));
                        return it;
                    } else {
                        std::promise<typename _traits::emplace_return> p;
                        Future<typename _traits::emplace_return> future = p.get_future();

                        value_type temp = TupleUnpacker::constructFromTuple<value_type>(std::move(args));
                        TupleUnpacker::forEach(std::forward<decltype(init)>(init), [&](auto &&f) { TupleUnpacker::invoke(f, temp); });
                        std::tuple<ContainerEvent, const_iterator, const value_type &> data { EMPLACE, where, temp };
                        this->writeRequest(&data, 0, 0, generateCallback(std::move(p), std::forward<decltype(onResult)>(onResult), std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(onFailure)>(onFailure)));

                        return future;
                    }
                }
            };
        }

        auto erase(const iterator &where)
        {
            return RequestBuilder {
                [=](auto &&onResult, auto &&onSuccess, auto &&onFailure) mutable -> Future<iterator> {
                    if (this->isMaster()) {
                        iterator it = erase_impl(where);
                        executeCallbacks(it, std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(onResult)>(onResult));
                        return it;
                    } else {
                        std::promise<iterator> p;
                        Future<iterator> future = p.get_future();

                        std::tuple<ContainerEvent, const_iterator> data { ERASE, where };

                        this->writeRequest(&data, 0, 0, generateCallback(std::move(p), std::forward<decltype(onResult)>(onResult), std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(onFailure)>(onFailure)));

                        return future;
                    }
                }
            };
        }

        auto erase(const iterator &from, const iterator &to)
        {
            iterator it = this->end();
            return RequestBuilder {
                [=](auto &&onResult, auto &&onSuccess, auto &&onFailure) mutable -> Future<iterator> {
                    if (this->isMaster()) {
                        iterator it = erase_impl(from, to);
                        executeCallbacks(it, std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(onResult)>(onResult));
                        return it;
                    } else {
                        std::promise<iterator> p;
                        Future<iterator> future = p.get_future();

                        std::tuple<ContainerEvent, const_iterator, const_iterator> data { ERASE_RANGE, from,
                            to };

                        this->writeRequest(&data, 0, 0, generateCallback(std::move(p), std::forward<decltype(onResult)>(onResult), std::forward<decltype(onSuccess)>(onSuccess), std::forward<decltype(onFailure)>(onFailure)));

                        return future;
                    }
                }
            };
        }

        struct _InsertOperation : Base::InsertOperation {
            _InsertOperation(container_t &c, const iterator &where, ParticipantId answerTarget = 0, TransactionId answerId = 0)
                : Base::InsertOperation(c, where)
                , mAnswerTarget(answerTarget)
                , mAnswerId(answerId)
            {
            }
            ~_InsertOperation()
            {
                assert(this->mCalled);
                if (this->mInserted) {
                    if (this->mContainer.isSynced()) {
                        std::tuple<ContainerEvent, const_iterator> data { EMPLACE, this->mIt };
                        container().writeAction(&data, mAnswerTarget, mAnswerId);
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

        struct _RemoveOperation : Base::RemoveOperation {
            _RemoveOperation(container_t &c, const iterator &it, ParticipantId answerTarget = 0, TransactionId answerId = 0)
                : Base::RemoveOperation(c, it)
            {
                if (this->mContainer.isSynced()) {
                    std::tuple<ContainerEvent, const_iterator> data { ERASE, it };
                    container().writeAction(&data, answerTarget, answerId);
                }
            }

            container_t &container()
            {
                return static_cast<container_t &>(this->mContainer);
            }
        };

        struct _RemoveRangeOperation : Base::RemoveRangeOperation {
            _RemoveRangeOperation(container_t &c, const iterator &from, const iterator &to, ParticipantId answerTarget = 0, TransactionId answerId = 0)
                : Base::RemoveRangeOperation(c, from, to)
            {
                if (this->mContainer.isSynced()) {
                    std::tuple<ContainerEvent, const_iterator, const_iterator> data { ERASE_RANGE, from, to };
                    this->writeAction(&data, answerTarget, answerId);
                }
            }

            container_t &container()
            {
                return static_cast<container_t &>(this->mContainer);
            }
        };

        struct _ResetOperation : Base::ResetOperation {
            _ResetOperation(container_t &c, bool controlled, ParticipantId answerTarget = 0, TransactionId answerId = 0)
                : Base::ResetOperation(c, controlled)
                , mAnswerTarget(answerTarget)
                , mAnswerId(answerId)
            {
            }
            ~_ResetOperation()
            {
                if (this->mContainer.isSynced()) {
                    std::tuple<ContainerEvent, const_iterator> data { RESET, container().end() };
                    container().writeAction(&data, mAnswerTarget, mAnswerId);
                }
            }

            container_t &container()
            {
                return static_cast<container_t &>(this->mContainer);
            }

            using Base::ResetOperation::operator=;

        private:
            ParticipantId mAnswerTarget;
            TransactionId mAnswerId;
        };

        using InsertOperation = AtomicContainerOperation<_InsertOperation>;
        using RemoveOperation = AtomicContainerOperation<_RemoveOperation>;
        using RemoveRangeOperation = AtomicContainerOperation<_RemoveRangeOperation>;
        using ResetOperation = AtomicContainerOperation<_ResetOperation>;
        //TODO: MultiInsertOperation

    private:
        template <typename T, typename... Callbacks>
        static void executeCallbacks(const T &t, Callbacks &&... callbacks)
        {
            (TupleUnpacker::forEach(std::forward<Callbacks>(callbacks), [&](auto &&f) { TupleUnpacker::invoke(f, t); }), ...);
        }

        template <typename T, typename Then, typename OnSuccess, typename OnFailure>
        static Lambda<void(void *)> generateCallback(std::promise<T> p, Then &&onResult, OnSuccess &&onSuccess, OnFailure &&onFailure)
        {
            return [p = std::move(p), onResult = std::forward<Then>(onResult), onSuccess = std::forward<OnSuccess>(onSuccess), onFailure = std::forward<OnFailure>(onFailure)](void *data) mutable {
                if (data) {
                    T *t = static_cast<T *>(data);
                    executeCallbacks(*t, std::move(onSuccess), std::move(onResult));
                    p.set_value(std::move(*t));
                } else {
                    TupleUnpacker::forEach(std::move(onFailure), [&](auto &&f) { std::forward<decltype(f)>(f)(); });
                    TupleUnpacker::forEach(std::move(onResult), [&](auto &&f) { TupleUnpacker::invoke(std::forward<decltype(f)>(f), std::nullopt); });
                }
            };
        }

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
    };

    template <typename C, typename Observer = NoOpFunctor, typename OffsetPtr = TaggedPlaceholder<MemberOffsetPtrTag, 0>>
    using SyncableContainer = container_api<SyncableContainerImpl<C, Observer, OffsetPtr>>;

#define SYNCABLE_CONTAINER(Name, ...) MEMBER_OFFSET_CONTAINER(Name, ::Engine::Serialize::SyncableContainer<__VA_ARGS__>)

}

template <typename C, typename Observer, typename _OffsetPtr>
struct underlying_container<Serialize::SyncableContainerImpl<C, Observer, _OffsetPtr>> {
    typedef C type;
};

template <typename C, typename Observer, typename _OffsetPtr>
struct container_traits<Serialize::SyncableContainerImpl<C, Observer, _OffsetPtr>> : container_traits<C> {
    typedef Serialize::SyncableContainerImpl<C, Observer, _OffsetPtr> container;

    using _traits = container_traits<C>;

    template <typename... Args>
    static typename _traits::emplace_return emplace(container &c, const typename _traits::iterator &where, Args &&... args)
    {
        return c.emplace(where, std::forward<Args>(args)...);
    }
};

}
