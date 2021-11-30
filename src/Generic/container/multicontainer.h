#pragma once

#include "../type_pack.h"
#include "../referencetuple.h"

namespace Engine {

template <template <typename...> typename Container, typename... Ty>
struct MultiContainer {

    using value_type = std::tuple<Ty...>;

    struct pointer;
    struct const_pointer;

    using reference = ReferenceTuple<Ty...>;
    using const_reference = ReferenceTuple<const Ty...>;

    struct pointer {
        ptrdiff_t operator-(const pointer &other) const
        {
            ptrdiff_t diff = std::get<0>(mData) - std::get<0>(other.mData);
            assert(((std::get<Ty *>(mData) - std::get<Ty *>(other.mData) == diff) && ...));
            return diff;
        }

        std::tuple<Ty *...> mData;
    };

    struct const_pointer {

        std::tuple<const Ty *...> mData;
    };

    template <typename It, bool isConst>
    struct IteratorImpl {

        using iterator_category = typename Container<std::tuple<Ty...>>::iterator::iterator_category;
        using value_type = typename MultiContainer<Container, Ty...>::value_type;
        using difference_type = ptrdiff_t;
        using pointer = std::conditional_t<isConst, typename MultiContainer<Container, Ty...>::const_pointer, typename MultiContainer<Container, Ty...>::pointer>;
        using reference = std::conditional_t<isConst, typename MultiContainer<Container, Ty...>::const_reference, typename MultiContainer<Container, Ty...>::reference>;

        IteratorImpl() = default;

        IteratorImpl(It it)
            : mIt(std::move(it))
        {
        }

        template <typename It2, bool isConst2>
        IteratorImpl(const IteratorImpl<It2, isConst2> &other)
            : mIt(other.mIt)
        {
        }

        IteratorImpl operator+(ptrdiff_t diff) const
        {
            return {
                TupleUnpacker::forEach(mIt, [=](auto &it) {
                    return it + diff;
                })
            };
        }

        IteratorImpl operator-(ptrdiff_t diff) const
        {
            return {
                TupleUnpacker::forEach(mIt, [=](auto &it) {
                    return it - diff;
                })
            };
        }

        ptrdiff_t operator-(const IteratorImpl &other) const
        {
            return std::get<0>(mIt) - std::get<0>(other.mIt);
        }

        constexpr bool operator!=(const IteratorImpl &other) const
        {
            return std::get<0>(mIt) != std::get<0>(other.mIt);
        }

        template <typename It2, bool isConst2>
        constexpr bool operator!=(const IteratorImpl<It2, isConst2> &other) const
        {
            return std::get<0>(mIt) != std::get<0>(other.mIt);
        }

        constexpr bool operator==(const IteratorImpl &other) const
        {
            return std::get<0>(mIt) == std::get<0>(other.mIt);
        }

                template <typename It2, bool isConst2>
        constexpr bool operator==(const IteratorImpl<It2, isConst2> &other) const
        {
            return std::get<0>(mIt) == std::get<0>(other.mIt);
        }

        IteratorImpl &operator++()
        {
            TupleUnpacker::forEach(mIt, [](auto &it) { ++it; });
            return *this;
        }

        IteratorImpl &operator--()
        {
            TupleUnpacker::forEach(mIt, [](auto &it) { --it; });
            return *this;
        }

        reference operator*() const
        {
            return { TupleUnpacker::forEach(mIt, [](auto &it) { return std::ref(*it); }) };
        }

        template <size_t I>
        friend decltype(auto) get(const IteratorImpl &it)
        {
            return std::get<I>(it.mIt);
        }

        template <size_t I>
        friend decltype(auto) get(IteratorImpl &it)
        {
            return std::get<I>(it.mIt);
        }

        It mIt;
    };

    using iterator = IteratorImpl<std::tuple<typename Container<Ty>::iterator...>, false>;
    using const_iterator = IteratorImpl<std::tuple<typename Container<Ty>::const_iterator...>, true>;
    using reverse_iterator = IteratorImpl<std::tuple<typename Container<Ty>::reverse_iterator...>, false>;
    using const_reverse_iterator = IteratorImpl<std::tuple<typename Container<Ty>::const_reverse_iterator...>, true>;

    reference at(size_t i)
    {
        return {
            { std::get<Container<Ty>>(mData).at(i)... }
        };
    }

    const_reference at(size_t i) const
    {
        return {
            { std::get<Container<Ty>>(mData).at(i)... }
        };
    }

    template <typename... Tuples>
    void construct(reference item, std::piecewise_construct_t, Tuples &&...tuples)
    {
        (TupleUnpacker::invokeFromTuple([&](auto... par){ new (&get<Ty>(item)) Ty(std::forward<decltype(par)>(par)...); }, std::forward<Tuples>(tuples)),...);   
    }

    void destruct(reference item)
    {
        (get<Ty>(item).~Ty(),...);
    }

    template <typename... Tuples>
    reference emplace_back(std::piecewise_construct_t, Tuples &&...tuples)
    {
        return { { TupleUnpacker::invokeFromTuple(LIFT(std::get<Container<Ty>>(mData).emplace_back, &), std::forward<Tuples>(tuples))... } };
    }

    template <typename... Tuples>
    iterator emplace(const const_iterator &where, std::piecewise_construct_t, Tuples &&...tuples)
    {
        return { { TupleUnpacker::invokeExpand(LIFT(std::get<Container<Ty>>(mData).emplace, &), std::get<typename Container<Ty>::const_iterator>(where.mIt), std::forward<Tuples>(tuples))... } };
    }

    iterator erase(const iterator &where)
    {
        return {
            { std::get<Container<Ty>>(mData).erase(std::get<typename Container<Ty>::iterator>(where.mIt))... }
        };
    }

    iterator begin()
    {
        return {
            TupleUnpacker::forEach(mData, [](auto &v) {
                return v.begin();
            })
        };
    }

    iterator end()
    {
        return {
            TupleUnpacker::forEach(mData, [](auto &v) {
                return v.end();
            })
        };
    }

    const_iterator begin() const
    {
        return {
            TupleUnpacker::forEach(mData, [](auto &v) {
                return v.begin();
            })
        };
    }

    const_iterator end() const
    {
        return {
            TupleUnpacker::forEach(mData, [](auto &v) {
                return v.end();
            })
        };
    }

    pointer data()
    {
        return {
            { std::get<Container<Ty>>(mData).data()... }
        };
    }

    reference front()
    {
        return {
            { std::get<Container<Ty>>(mData).front()... }
        };
    }

    size_t size() const
    {
        return std::get<0>(mData).size();
    }

    bool empty() const
    {
        return std::get<0>(mData).empty();
    }

    reference operator[](size_t i)
    {
        return {
            { std::get<Container<Ty>>(mData)[i]... }
        };
    }

    void clear()
    {
        (std::get<Container<Ty>>(mData).clear(), ...);
    }

    template <size_t I>
    friend decltype(auto) get(MultiContainer &data)
    {
        return std::get<I>(data.mData);
    }

private:
    friend struct container_traits<MultiContainer<Container, Ty...>, void>;

    std::tuple<Container<Ty>...> mData;
};

template <template <typename...> typename Container, typename... Ty>
struct container_traits<MultiContainer<Container, Ty...>> {

    typedef MultiContainer<Container, Ty...> container;
    typedef typename container::value_type value_type;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::reverse_iterator reverse_iterator;
    typedef typename container::const_reverse_iterator const_reverse_iterator;

    typedef Pib<iterator> emplace_return;

    using helper_traits = container_traits<Container<first_t<Ty...>>>;

    using handle = typename helper_traits::handle;
    static_assert((std::same_as<handle, typename container_traits<Container<Ty>>::handle> && ...));
    using position_handle = typename helper_traits::position_handle;
    static_assert((std::same_as<position_handle, typename container_traits<Container<Ty>>::position_handle> && ...));
    using const_handle = typename helper_traits::const_handle;
    static_assert((std::same_as<const_handle, typename container_traits<Container<Ty>>::const_handle> && ...));
    using const_position_handle = typename helper_traits::const_position_handle;
    static_assert((std::same_as<const_position_handle, typename container_traits<Container<Ty>>::const_position_handle> && ...));

    template <typename... _Ty>
    static emplace_return emplace(container &c, const const_iterator &where, _Ty &&...args)
    {
        return c.emplace(where, std::forward<_Ty>(args)...);
    }

    static bool was_emplace_successful(const emplace_return &ret)
    {
        return ret.success();
    }

    static position_handle toPositionHandle(container &c, const iterator &it)
    {
        return helper_traits::toPositionHandle(get<0>(c), get<0>(it));
    }

    static handle toHandle(container &c, const iterator &it)
    {
        return helper_traits::toHandle(get<0>(c), get<0>(it));
    }

    static void revalidateHandleAfterInsert(position_handle &handle, const container &c, const const_iterator &it)
    {
        helper_traits::revalidateHandleAfterInsert(handle, get<0>(c), get<0>(it));
    }

    static void revalidateHandleAfterRemove(position_handle &handle, const container &c, const const_iterator &it, bool wasIn, size_t count = 1)
    {
        helper_traits::revalidateHandleAfterRemove(handle, get<0>(c), get<0>(it));
    }

    static iterator toIterator(container &c, const position_handle &handle)
    {
        return { { container_traits<Container<Ty>>::toIterator(std::get<Container<Ty>>(c.mData), handle)... } };
    }

    static const_iterator toIterator(const container &c, const const_position_handle &handle)
    {
        return { container_traits<Container<Ty>>::toIterator(std::get<Container<Ty>>(c.mData), handle)... };
    }

    static position_handle next(container &c, const position_handle &handle)
    {
        return toPositionHandle(c, ++toIterator(c, handle));
    }

    static position_handle prev(container &c, const position_handle &handle)
    {
        return toPositionHandle(c, --toIterator(c, handle));
    }
};

template <typename C, template <typename...> typename Container, typename... Ty>
struct container_api_impl<C, MultiContainer<Container, Ty...>> : C {

    using C::C;

    using C::operator=;

    using value_type = typename C::value_type;
    using reference = typename C::reference;
    using const_reference = typename C::const_reference;

    void remove(const value_type &item)
    {
        for (typename C::const_iterator it = this->begin(); it != this->end();) {
            if (*it == item) {
                it = erase(it);
            } else {
                ++it;
            }
        }
    }

    decltype(auto) push_back(const value_type &item)
    {
        return then(emplace_back(item), [](auto &&it) -> decltype(auto) {
            return *it;
        });
    }

    decltype(auto) push_back(value_type &&item)
    {
        return then(emplace_back(std::move(item)), [](auto &&it) -> decltype(auto) {
            return *it;
        });
    }

    template <typename... _Ty>
    decltype(auto) emplace_back(_Ty &&...args)
    {
        return C::emplace_back(std::forward<_Ty>(args)...);
    }

    reference at(size_t i)
    {
        return C::at(i);
    }

    const_reference at(size_t i) const
    {
        return C::at(i);
    }

    reference operator[](size_t i)
    {
        return C::operator[](i);
    }

    const_reference operator[](size_t i) const
    {
        return C::operator[](i);
    }
};
}