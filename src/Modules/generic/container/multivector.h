#pragma once

namespace Engine {

template <typename... Ty>
struct MultiVector {

    using first_t = type_pack_select_t<0, type_pack<Ty...>>;
    using value_type = std::tuple<Ty...>;

    struct pointer;
    struct const_pointer;

    struct reference {

        template <size_t I>
        decltype(auto) get()
        {
            return std::get<I>(mData);
        }

        pointer operator&() const
        {
            return {
                { &std::get<Ty &>(mData)... }
            };
        }

        friend void swap(const reference &first, const reference &second)
        {
            (std::swap(std::get<Ty &>(first.mData), std::get<Ty &>(second.mData)), ...);
        }

        std::tuple<Ty &...> mData;
    };

    struct const_reference {

        template <size_t I>
        decltype(auto) get()
        {
            return std::get<I>(mData);
        }

        std::tuple<const Ty &...> mData;
    };

    struct pointer {
        ptrdiff_t operator-(const pointer &other) const
        {
            ptrdiff_t diff = std::get<0>(mData) - std::get<0>(other.mData);
            assert((std::get<Ty *>(mData) - std::get<Ty *>(other.mData) == diff) && ...);
            return diff;
        }

        std::tuple<Ty *...> mData;
    };

    struct const_pointer {

        std::tuple<const Ty *...> mData;
    };

    struct iterator {

        using iterator_category = typename std::vector<first_t>::iterator::iterator_category;
        using value_type = typename MultiVector<Ty...>::value_type;
        using difference_type = ptrdiff_t;
        using pointer = typename MultiVector<Ty...>::pointer;
        using reference = typename MultiVector<Ty...>::reference;

        size_t mIndex;
    };

    reference at(size_t i)
    {
        return {
            { std::get<std::vector<Ty>>(mData).at(i)... }
        };
    }

    const_reference at(size_t i) const
    {
        return {
            { std::get<std::vector<Ty>>(mData).at(i)... }
        };
    }

    template <typename... Tuples>
    reference emplace_back(std::piecewise_construct_t, Tuples &&... tuples)
    {
        return { { TupleUnpacker::invokeFromTuple(LIFT(std::get<std::vector<Ty>>(mData).emplace_back, &), std::forward<Tuples>(tuples))... } };
    }

    reference front()
    {
        return {
            { std::get<std::vector<Ty>>(mData).front()... }
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
            { std::get<std::vector<Ty>>(mData)[i]... }
        };
    }

    void pop_back()
    {
        (std::get<std::vector<Ty>>(mData).pop_back(), ...);
    }

    void clear()
    {
        (std::get<std::vector<Ty>>(mData).clear(), ...);
    }

    template <size_t I>
    decltype(auto) get() {
        return std::get<I>(mData);
    }

private:
    std::tuple<std::vector<Ty>...> mData;
};


template <typename C, typename... Ty>
struct container_api_impl<C, MultiVector<Ty...>> : C {

    using C::C;

    using C::operator=;

    using value_type = typename C::value_type;
    using reference = typename C::reference;
    using const_reference = typename C::const_reference;

    /*void resize(size_t size)
            {
                C::resize(size);
            }*/

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
    decltype(auto) emplace_back(_Ty &&... args)
    {
        return this->emplace(this->end(), std::forward<_Ty>(args)...);
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

    template <size_t I>
    decltype(auto) get() {
        return C::template get<I>();
    }
    
};



}