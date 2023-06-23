#pragma once

#include "serializable.h"
#include "syncable.h"
#include "Generic/memberoffsetptr.h"

//TODO rewrite to modern operations
namespace Engine {
namespace Serialize {

#define SYNCED(Name, ...) MEMBER_OFFSET_CONTAINER(Name,, ::Engine::Serialize::Synced<__VA_ARGS__>)
#define SYNCED_INIT(Name, Init, ...) MEMBER_OFFSET_CONTAINER(Name, Init, ::Engine::Serialize::Synced<__VA_ARGS__>)

    template <typename T, typename Observer = NoOpFunctor, typename OffsetPtr = TaggedPlaceholder<MemberOffsetPtrTag, 0>>
    struct Synced : Syncable<OffsetPtr>, Serializable<OffsetPtr>, private Observer {

        friend struct Operations<Synced<T, Observer, OffsetPtr>>;

        enum class Operation {
            SET,
            ADD,
            SUB
        };

        struct action_payload {
            Operation mOperation;
            T mValue;
        };

        struct request_payload {
            Operation mOperation;
            T mValue;
        };

        template <typename... _Ty>
        Synced(_Ty &&...args)
            : mData(std::forward<_Ty>(args)...)
        {
        }

        Observer &observer()
        {
            return *this;
        }

        template <typename Ty>
        void operator=(Ty &&v)
        {
            if (mData != v) {
                if (this->isMaster()) {
                    T old = mData;
                    mData = std::forward<Ty>(v);
                    notify(old);
                } else {
                    this->writeRequest({}, Operation::SET, static_cast<T>(std::forward<Ty>(v)));
                }
            }
        }

        template <typename Ty>
        void operator+=(Ty &&v)
        {
            if (this->isMaster()) {
                T old = mData;
                mData += std::forward<Ty>(v);
                notify(old);
            } else {
                this->writeRequest({}, Operation::ADD, static_cast<T>(std::forward<Ty>(v)));
            }
        }

        template <typename Ty>
        void operator-=(Ty &&v)
        {
            if (this->isMaster()) {
                T old = mData;
                mData -= std::forward<Ty>(v);
                notify(old);
            } else {
                this->writeRequest({}, Operation::SUB, static_cast<T>(std::forward<Ty>(v)));
            }
        }

        T *operator->()
        {
            return &mData;
        }

        T *ptr()
        {
            return &mData;
        }

        operator const T &() const
        {
            return mData;
        }

    protected:
        void notify(const T &old, ParticipantId answerTarget = 0, MessageId answerId = 0)
        {
            if (this->isSynced()) {
                this->writeAction(answerTarget, answerId, Operation::SET, mData);
            }
            if (this->isActive()) {
                Observer::operator()(mData, old);
            }
        }

    private:
        T mData;
    };
}
}
