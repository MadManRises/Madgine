#pragma once

#include "../generic/functor.h"
#include "../generic/transformIt.h"

namespace Engine {

template <typename T, typename = void>
struct KeyValue {
    static T &value(T &v)
    {
        return v;
    }

    static T value(T &&v)
    {
        return std::move(v);
    }

    static std::monostate key(T &v)
    {
        return {};
    }

    static std::monostate key(T &&v)
    {
        return {};
    }
};

template <typename T>
struct KeyValue<T, std::void_t<std::invoke_result_t<decltype(&T::key), T *>>> {
    static T &value(T &v)
    {
        return v;
    }

    static std::invoke_result_t<decltype(&T::key), T *> key(const T &v)
    {
        return v.key();
    }
};

template <typename T>
struct KeyValue<T *> {
    static T *value(T *v)
    {
        return v;
    }

    static decltype(auto) key(const T *v)
    {
        return v->key();
    }
};

template <typename T>
struct KeyValue<T *const> {
    static T *value(T *v)
    {
        return v;
    }

    static decltype(auto) key(const T *v)
    {
        return v->key();
    }
};

template <typename K, typename T>
struct KeyValue<std::pair<K, T>> {
    static T &value(std::pair<K, T> &p)
    {
        return p.second;
    }

    static const K &key(const std::pair<K, T> &p)
    {
        return p.first;
    }
};

template <typename K, typename T>
struct KeyValue<const std::pair<K, T>> {
    static const T &value(const std::pair<K, T> &p)
    {
        return p.second;
    }

    static const K &key(const std::pair<K, T> &p)
    {
        return p.first;
    }
};

template <typename T, typename D>
struct KeyValue<std::unique_ptr<T, D>> {
    static T *value(const std::unique_ptr<T, D> &p) // TODO
    {
        return p.get();
    }

    static decltype(auto) key(const std::unique_ptr<T, D> &p)
    {
        return KeyValue<T>::key(*p);
    }
};

template <typename T, typename D>
struct KeyValue<const std::unique_ptr<T, D>> {
    static T *value(const std::unique_ptr<T, D> &p) // TODO
    {
        return p.get();
    }

    static decltype(auto) key(const std::unique_ptr<T, D> &p)
    {
        return KeyValue<T>::key(*p);
    }
};

template <typename T>
decltype(auto) kvValue(T &&v)
{
    return KeyValue<std::remove_reference_t<T>>::value(std::forward<T>(v));
}

template <typename T>
decltype(auto) kvKey(T &&v)
{
    return KeyValue<std::remove_reference_t<T>>::key(std::forward<T>(v));
}

template <typename T>
decltype(auto) kvValues(T &v)
{
    return transformIt<Functor<kvValue<decltype(*v.begin())>>>(v);
}

template <typename T>
decltype(auto) kvKeys(T &v)
{
    return transformIt<Functor<kvKey<decltype(*v.begin())>>>(v);
}

template <typename T, typename K>
struct Finder {
    static auto find(T &c, const K &key)
    {
        return std::find_if(c.begin(), c.end(), [&](const typename T::value_type &v) { return kvKey(v) == key; });
    }
};

template <typename V, typename K, typename _K>
struct Finder<std::map<K, V>, _K> {
    static auto find(std::map<K, V> &c, const _K &key)
    {
        return c.find(key);
    }
};

template <typename V, typename K, typename _K>
struct Finder<const std::map<K, V>, _K> {
    static auto find(const std::map<K, V> &c, const _K &key)
    {
        return c.find(key);
    }
};

template <typename T, typename K>
decltype(auto) kvFind(T &c, const K &key)
{
    return Finder<T, K>::find(c, key);
}

template <typename T>
struct KeyType {
    typedef std::decay_t<decltype(kvKey(std::declval<T>()))> type;
};

template <typename T>
using KeyType_t = typename KeyType<T>::type;

template <typename _Ty>
struct KeyCompare {

    struct traits {
        typedef KeyCompare<_Ty> cmp_type;
        typedef KeyType_t<_Ty> type;
        typedef _Ty item_type;

        static decltype(auto) to_cmp_type(const _Ty &v)
        {
            return kvKey(v);
        }
    };

    using is_transparent = void;

    constexpr bool operator()(const _Ty &_Left, const _Ty &_Right) const
    {
        // apply operator< to operands
        return (kvKey(_Left) < kvKey(_Right));
    }

    constexpr bool operator()(const KeyType_t<_Ty> &_Left, const _Ty &_Right) const
    {
        return (_Left < kvKey(_Right));
    }

    constexpr bool operator()(const _Ty &_Left, const KeyType_t<_Ty> &_Right) const
    {
        return (kvKey(_Left) < _Right);
    }
};

template <typename _Ty = void>
struct KeyHash {
    typedef _Ty argument_type;
    typedef size_t result_type;

    constexpr size_t operator()(const _Ty &_Arg) const
    {
        return std::hash<KeyType_t<_Ty>> {}(kvKey(_Arg));
    }
};

}
