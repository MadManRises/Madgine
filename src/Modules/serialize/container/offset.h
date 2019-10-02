#pragma once

#include "../../generic/offsetptr.h"

template <typename T, size_t L>
extern Engine::OffsetPtr<T, Engine::Serialize::SyncableBase> SyncableOffsetInstance();

template <typename T, size_t L>
extern Engine::OffsetPtr<T, Engine::Serialize::SerializableBase> SerializableOffsetInstance();

namespace Engine {
namespace Serialize {

    template <typename T, size_t L>
    struct SyncableOffsetPtr {
        static T *parent(SyncableBase *o)
        {
            return SyncableOffsetInstance<T, L>().parent(o);
        }

        static const T *parent(const SyncableBase *o)
        {
            return SyncableOffsetInstance<T, L>().parent(o);
        }

        template <typename _T = T, typename _M = SyncableBase>
        static size_t offset()
        {
            return SyncableOffsetInstance<T, L>().template offset<_T, _M>();
        }
    };

#define DEFINE_SYNCABLE_OFFSET(Name)                                                                             \
    friend ::Engine::OffsetPtr<Self, ::Engine::Serialize::SyncableBase> SyncableOffsetInstance<Self, __LINE__>() \
    {                                                                                                            \
        return { &Self::Name };                                                                                  \
    }

    template <typename T, size_t L>
    struct SerializableOffsetPtr {
        static T *parent(SerializableBase *o)
        {
            return SerializableOffsetInstance<T, L>().parent(o);
        }

        static const T *parent(const SerializableBase *o)
        {
            return SerializableOffsetInstance<T, L>().parent(o);
        }

        template <typename _T = T, typename _M = SerializableBase>
        static size_t offset()
        {
            return SerializableOffsetInstance<T, L>().template offset<_T, _M>();
        }
    };

#define DEFINE_SERIALIZABLE_OFFSET(Name)                                                                                 \
    friend ::Engine::OffsetPtr<Self, ::Engine::Serialize::SerializableBase> SerializableOffsetInstance<Self, __LINE__>() \
    {                                                                                                                    \
        return { &Self::Name };                                                                                          \
    }

    template <typename T, size_t L>
    struct CombinedOffsetPtr : SerializableOffsetPtr<T, L>, SyncableOffsetPtr<T, L> {

        template <typename Ty>
        static T *parent(Ty *o)
        {
            if constexpr (std::is_convertible_v<Ty &, SerializableBase &>)
                return SerializableOffsetPtr<T, L>::parent(o);
            else
                return SyncableOffsetPtr<T, L>::parent(o);
        }

        template <typename Ty>
        static const T *parent(const Ty *o)
        {
            if constexpr (std::is_convertible_v<Ty &, SerializableBase &>)
                return SerializableOffsetPtr<T, L>::parent(o);
            else
                return SyncableOffsetPtr<T, L>::parent(o);
        }

        template <typename _T, typename _M>
        static size_t offset()
        {
            if constexpr (std::is_convertible_v<_M &, SyncableBase &>)
                return SyncableOffsetPtr<T, L>::template offset<_T, _M>();
            else
                return SerializableOffsetPtr<T, L>::template offset<_T, _M>();
        }
    };

#define DEFINE_COMBINED_OFFSET(Name) DEFINE_SERIALIZABLE_OFFSET(Name) \
DEFINE_SYNCABLE_OFFSET(Name)

}
}