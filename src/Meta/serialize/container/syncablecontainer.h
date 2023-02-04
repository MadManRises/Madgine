#pragma once

#include "serializablecontainer.h"
#include "syncable.h"

#include "Generic/container/containerevent.h"

namespace Engine {
namespace Serialize {

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

                this->writeRequest(0, 0, &temp); //??? Is the temp object used?
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
                this->writeRequest(0, 0, &op);
            }
        }

        template <typename... _Ty>
        auto emplace(const iterator &where, _Ty &&...args)
        {
            return make_message_sender<typename _traits::emplace_return>(
                [this](auto &receiver, const iterator &where, _Ty &&...args) {
                    if (this->isMaster()) {
                        receiver.set_value(emplace_impl(where, std::forward<_Ty>(args)...));
                    } else {
                        value_type temp { std::forward<_Ty>(args)... };
                        this->writeRequest(receiver, EMPLACE, where, std::ref(temp));
                    }
                },
                where,
                std::forward<_Ty>(args)...);
        }

        template <typename Init, typename... _Ty>
        auto emplace_init(const iterator &where, Init &&init, _Ty &&...args)
        {
            return make_message_sender<typename _traits::emplace_return>(
                [this](auto &receiver, const iterator &where, Init &&init, _Ty &&...args) {
                    if (this->isMaster()) {
                        receiver.set_value(emplace_impl_init(where, std::forward<decltype(init)>(init), std::forward<_Ty>(args)...));
                    } else {
                        value_type temp { std::forward<_Ty>(args)... };
                        TupleUnpacker::invoke(std::forward<Init>(init), temp);
                        this->writeRequest(receiver, EMPLACE, where, std::ref(temp));
                    }
                },
                where, std::forward<Init>(init), std::forward<_Ty>(args)...);
        }

        auto erase(const iterator &where)
        {
            return make_message_sender<iterator>(
                [this](auto &receiver, const iterator &where) {
                    if (this->isMaster()) {
                        receiver.set_value(erase_impl(where));
                    } else {
                        this->writeRequest(receiver, ERASE, where);
                    }
                },
                where);
        }

        auto erase(const iterator &from, const iterator &to)
        {
            return make_message_sender<iterator>(
                [this](auto &receiver, const iterator &from, const iterator &to) {
                    iterator it = this->end();
                    if (this->isMaster()) {
                        receiver.set_value(erase_impl(from, to));
                    } else {
                        this->writeRequest(receiver, ERASE_RANGE, from, to);
                    }
                },
                from, to);
        }

        struct _InsertOperation : Base::InsertOperation {
            _InsertOperation(container_t &c, const iterator &where, ParticipantId answerTarget = 0, MessageId answerId = 0)
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
                        container().writeAction(mAnswerTarget, mAnswerId, EMPLACE, this->mIt);
                    }
                }
            }

            container_t &container()
            {
                return static_cast<container_t &>(this->mContainer);
            }

        private:
            ParticipantId mAnswerTarget;
            MessageId mAnswerId;
        };

        struct _RemoveOperation : Base::RemoveOperation {
            _RemoveOperation(container_t &c, const iterator &it, ParticipantId answerTarget = 0, MessageId answerId = 0)
                : Base::RemoveOperation(c, it)
            {
                if (this->mContainer.isSynced()) {
                    container().writeAction(answerTarget, answerId, ERASE, it);
                }
            }

            container_t &container()
            {
                return static_cast<container_t &>(this->mContainer);
            }
        };

        struct _RemoveRangeOperation : Base::RemoveRangeOperation {
            _RemoveRangeOperation(container_t &c, const iterator &from, const iterator &to, ParticipantId answerTarget = 0, MessageId answerId = 0)
                : Base::RemoveRangeOperation(c, from, to)
            {
                if (this->mContainer.isSynced()) {
                    this->writeAction(answerTarget, answerId, ERASE_RANGE, from, to);
                }
            }

            container_t &container()
            {
                return static_cast<container_t &>(this->mContainer);
            }
        };

        struct _ResetOperation : Base::ResetOperation {
            _ResetOperation(container_t &c, bool controlled, ParticipantId answerTarget = 0, MessageId answerId = 0)
                : Base::ResetOperation(c, controlled)
                , mAnswerTarget(answerTarget)
                , mAnswerId(answerId)
            {
            }
            ~_ResetOperation()
            {
                if (this->mContainer.isSynced()) {
                    container().writeAction(mAnswerTarget, mAnswerId, RESET, container().end());
                }
            }

            container_t &container()
            {
                return static_cast<container_t &>(this->mContainer);
            }

            using Base::ResetOperation::operator=;

        private:
            ParticipantId mAnswerTarget;
            MessageId mAnswerId;
        };

        using InsertOperation = AtomicContainerOperation<_InsertOperation>;
        using RemoveOperation = AtomicContainerOperation<_RemoveOperation>;
        using RemoveRangeOperation = AtomicContainerOperation<_RemoveRangeOperation>;
        using ResetOperation = AtomicContainerOperation<_ResetOperation>;
        //TODO: MultiInsertOperation

    private:
        template <typename Init, typename... _Ty>
        typename _traits::emplace_return emplace_impl_init(const iterator &where, Init &&init, _Ty &&...args)
        {
            InsertOperation op { *this, where };
            typename _traits::emplace_return it = op.emplace(where, std::forward<_Ty>(args)...);
            if (_traits::was_emplace_successful(it)) {
                init(*it);
            }
            return it;
        }

        template <typename... _Ty>
        typename _traits::emplace_return emplace_impl(const iterator &where, _Ty &&...args)
        {
            InsertOperation op { *this, where };
            typename _traits::emplace_return it = op.emplace(where, std::forward<_Ty>(args)...);
            return it;
        }

        iterator erase_impl(const iterator &it, ParticipantId answerTarget = 0, MessageId answerId = 0)
        {
            return RemoveOperation { *this, it, answerTarget, answerId }.erase(it);
        }

        iterator erase_impl(const iterator &from, const iterator &to, ParticipantId answerTarget = 0, MessageId answerId = 0)
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
    static typename _traits::emplace_return emplace(container &c, const typename _traits::iterator &where, Args &&...args)
    {
        return c.emplace(where, std::forward<Args>(args)...);
    }
};

}
