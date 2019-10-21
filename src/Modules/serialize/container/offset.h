#pragma once

#include "../../generic/offsetptr.h"

namespace Engine {
namespace Serialize {

    template <typename T, auto P>
    struct SyncableOffsetPtr {
        static T *parent(SyncableBase *o)
        {
            return P().parent(o);
        }

        static const T *parent(const SyncableBase *o)
        {
            return P->parent(o);
        }

        template <typename _T = T, typename _M = SyncableBase>
        static size_t offset()
        {
            return P->template offset<_T, _M>();
        }
    };

#define DECLARE_SYNCABLE_OFFSET(Name) \
    static inline ::Engine::OffsetPtr<Self, ::Engine::Serialize::SyncableBase> __access_syncable_##Name##__;
    //friend ::Engine::OffsetPtr<Self, ::Engine::Serialize::SerializableBase> __access_serializable_##Name##__();
#define DEFINE_SYNCABLE_OFFSET(Name)                        \
    struct __access_syncable_##Name##_define__ {            \
        __access_syncable_##Name##_define__()               \
        {                                                   \
            __access_syncable_##Name##__ = { &Self::Name }; \
        }                                                   \
    };                                                      \
    static inline __access_syncable_##Name##_define__ __access_syncable_##Name##_definer__;
    /*friend ::Engine::OffsetPtr<Self, ::Engine::Serialize::SerializableBase> __access_serializable_##Name##__() \
    {                                                                                                        \
        return { &Self::Name };                                                                              \
    }*/
#define SYNCABLE_OFFSET(Name) \
    ::Engine::Serialize::SyncableOffsetPtr<Self, &__access_syncable_##Name##__>
    //::Engine::Serialize::SerializableOffsetPtr<Self, &__access_serializable_##Name##__>

    /*#define DECLARE_SYNCABLE_OFFSET(Name) \
    friend ::Engine::OffsetPtr<Self, ::Engine::Serialize::SyncableBase> __access_syncable_##Name##__();
#define DEFINE_SYNCABLE_OFFSET(Name)                                                                   \
    friend ::Engine::OffsetPtr<Self, ::Engine::Serialize::SyncableBase> __access_syncable_##Name##__() \
    {                                                                                                  \
        return { &Self::Name };                                                                        \
    }
#define SYNCABLE_OFFSET(Name) \
    ::Engine::Serialize::SyncableOffsetPtr<Self, &__access_syncable_##Name##__>*/

    template <typename T, auto P>
    struct SerializableOffsetPtr {
        static T *parent(SerializableBase *o)
        {
            return P->parent(o);
        }

        static const T *parent(const SerializableBase *o)
        {
            return P->parent(o);
        }

        template <typename _T = T, typename _M = SerializableBase>
        static size_t offset()
        {
            return P->template offset<_T, _M>();
        }
    };

#define DECLARE_SERIALIZABLE_OFFSET(Name) \
    static inline ::Engine::OffsetPtr<Self, ::Engine::Serialize::SerializableBase> __access_serializable_##Name##__;
    //friend ::Engine::OffsetPtr<Self, ::Engine::Serialize::SerializableBase> __access_serializable_##Name##__();
#define DEFINE_SERIALIZABLE_OFFSET(Name)             \
    struct __access_serializable_##Name##_define__ { \
 __access_serializable_##Name##_define__()               \
        {                                                   \
            __access_serializable_##Name##__ = { &Self::Name }; \
        }                                                   \
    };                                               \
    static inline __access_serializable_##Name##_define__ __access_serializable_##Name##_definer__;
    /*friend ::Engine::OffsetPtr<Self, ::Engine::Serialize::SerializableBase> __access_serializable_##Name##__() \
    {                                                                                                        \
        return { &Self::Name };                                                                              \
    }*/
#define SERIALIZABLE_OFFSET(Name) \
    ::Engine::Serialize::SerializableOffsetPtr<Self, &__access_serializable_##Name##__>
    //::Engine::Serialize::SerializableOffsetPtr<Self, &__access_serializable_##Name##__>

    template <typename T, auto Serialize, auto Sync>
    struct CombinedOffsetPtr : SerializableOffsetPtr<T, Serialize>, SyncableOffsetPtr<T, Sync> {

        template <typename Ty>
        static T *parent(Ty *o)
        {
            if constexpr (std::is_convertible_v<Ty &, SerializableBase &>)
                return SerializableOffsetPtr<T, Serialize>::parent(o);
            else
                return SyncableOffsetPtr<T, Sync>::parent(o);
        }

        template <typename Ty>
        static const T *parent(const Ty *o)
        {
            if constexpr (std::is_convertible_v<Ty &, SerializableBase &>)
                return SerializableOffsetPtr<T, Serialize>::parent(o);
            else
                return SyncableOffsetPtr<T, Sync>::parent(o);
        }

        template <typename _T, typename _M>
        static size_t offset()
        {
            if constexpr (std::is_convertible_v<_M &, SyncableBase &>)
                return SyncableOffsetPtr<T, Sync>::template offset<_T, _M>();
            else
                return SerializableOffsetPtr<T, Serialize>::template offset<_T, _M>();
        }
    };

#define DECLARE_COMBINED_OFFSET(Name) \
    DECLARE_SERIALIZABLE_OFFSET(Name) \
    DECLARE_SYNCABLE_OFFSET(Name)
#define DEFINE_COMBINED_OFFSET(Name) \
    DEFINE_SERIALIZABLE_OFFSET(Name) \
    DEFINE_SYNCABLE_OFFSET(Name)
#define COMBINED_OFFSET(Name) \
    ::Engine::Serialize::CombinedOffsetPtr<Self, &__access_serializable_##Name##__, &__access_syncable_##Name##__>

}
}