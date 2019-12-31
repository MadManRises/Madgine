#pragma once

namespace Engine {

struct OffsetPtrTag;

template <typename T, typename M>
struct OffsetPtr {

    using member_type = M;
    using parent_type = T;

    OffsetPtr() = default;

    template <typename Target>
    OffsetPtr(Target(T::*P))
    {
        mOffset = reinterpret_cast<size_t>(&static_cast<M &>(static_cast<T *>(reinterpret_cast<void *>(0x1))->*P)) - 1;
    }

    T *parent(M *m)
    {
        return reinterpret_cast<T *>(reinterpret_cast<char *>(m) - mOffset);
    }

    const T *parent(const M *m)
    {
        return reinterpret_cast<const T *>(reinterpret_cast<const char *>(m) - mOffset);
    }

    template <typename _T, typename _M = M>
    size_t offset()
    {
        size_t offset = mOffset;
        offset += reinterpret_cast<size_t>(static_cast<T *>(static_cast<_T *>(reinterpret_cast<void *>(0x1)))) - 1;
        offset += reinterpret_cast<size_t>(static_cast<_M *>(static_cast<M *>(reinterpret_cast<void *>(0x1)))) - 1;
        return offset;
    }

    size_t mOffset = -1;
};

template <typename P>
struct OffsetPtrWrapper {

    using ptr_type = decltype(P::value);
    using member_type = typename ptr_type::member_type;
    using parent_type = typename ptr_type::parent_type;

    template <typename Ty>
    static parent_type *parent(Ty *child)
    {
        return P::value.parent(static_cast<member_type *>(child));
    }

    template <typename Ty>
    static const parent_type *parent(const Ty *child)
    {
        return P::value.parent(static_cast<const member_type *>(child));
    }

    template <typename _T, typename _M = member_type>
    static size_t offset()
    {
        return P::value.template offset<_T, _M>();
    }
};

#define DECLARE_OFFSET(Name) \
    struct __access_##Name##__;

#define DEFINE_OFFSET(Name)                                                                   \
    static ::Engine::OffsetPtr<Self, decltype(Name)> __##Name##_value__()                     \
    {                                                                                         \
        return &Self::Name;                                                                   \
    }                                                                                         \
    struct __access_##Name##__ {                                                              \
        static inline ::Engine::OffsetPtr<Self, decltype(Name)> value = __##Name##_value__(); \
    };

#define OFFSET_CONTAINER(Name, ...)                                                                                                \
    DECLARE_OFFSET(Name)                                                                                                           \
    typename ::Engine::replace<__VA_ARGS__>::tagged<::Engine::OffsetPtrTag, ::Engine::OffsetPtrWrapper<__access_##Name##__>> Name; \
    DEFINE_OFFSET(Name)

}