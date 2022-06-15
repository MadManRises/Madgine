#pragma once

#include "syncable.h"
#include "serializable.h"

//TODO rewrite to modern operations
namespace Engine {
namespace Serialize {

    DERIVE_FUNCTION2(PlusAssign, operator+=)
    DERIVE_FUNCTION2(MinusAssign, operator-=)

#define SYNCED(Name, ...) MEMBER_OFFSET_CONTAINER(Name, ::Engine::Serialize::Synced<__VA_ARGS__>)

    template <typename T, typename Observer = NoOpFunctor, typename OffsetPtr = TaggedPlaceholder<MemberOffsetPtrTag, 0>>
    struct Synced : Syncable<OffsetPtr>, Serializable<OffsetPtr>, private Observer {

        friend struct Operations<Synced<T, Observer, OffsetPtr>>;

        enum class Operation {
            SET,
            ADD,
            SUB
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
                    std::pair<Operation, T> data { Operation::SET, std::forward<Ty>(v) };
                    this->writeRequest(&data);
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
                std::pair<Operation, T> data { Operation::ADD, std::forward<Ty>(v) };
                this->writeRequest(&data);
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
                std::pair<Operation, T> data { Operation::SUB, std::forward<Ty>(v) };
                this->writeRequest(, &data);
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
                std::pair<Operation, T> data { Operation::SET, mData };
                this->writeAction(&data, answerTarget, answerId);
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
