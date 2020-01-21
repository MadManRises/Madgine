#pragma once

#include "container_traits.h"
#include "observerevent.h"

namespace Engine {

template <typename C, typename Observer>
struct ObservableContainer : C, Observer {

    using _traits = container_traits<C>;
    struct traits : _traits {
        typedef ObservableContainer<C, Observer> container;

        typedef typename _traits::iterator iterator;
        typedef typename _traits::const_iterator const_iterator;

        template <typename... _Ty>
        static std::pair<iterator, bool> emplace(container &c, const iterator &where, _Ty &&... args)
        {
            return c.emplace(where, std::forward<_Ty>(args)...);
        }
    };

    typedef typename traits::iterator iterator;
    typedef typename traits::const_iterator const_iterator;

    template <typename... _Ty>
    std::pair<iterator, bool> emplace(const iterator &where, _Ty &&... args)
    {
        Observer::operator()(where, BEFORE | INSERT_ITEM);
        std::pair<iterator, bool> it = _traits::emplace(*this, where, std::forward<_Ty>(args)...);
        Observer::operator()(it.first, (it.second ? AFTER : ABORTED) | INSERT_ITEM);
        return it;
    }

    iterator erase(const iterator &where)
    {
        Observer::operator()(where, BEFORE | REMOVE_ITEM);
        iterator it = _traits::container::erase(where);
        Observer::operator()(it, AFTER | REMOVE_ITEM);
        return it;
    }

    iterator erase(const iterator &from, const iterator &to)
    {
        Observer::operator()(from, BEFORE | REMOVE_RANGE);
        size_t count = 0;
        for (iterator it = from; it != to; ++it) {
            Observer::operator()(it, BEFORE | REMOVE_ITEM);
            ++count;
        }
        iterator it = _traits::container::erase(from, to);
        for (; count > 0; --count) {
            Observer::operator()(it, AFTER | REMOVE_ITEM);
        }
        Observer::operator()(it, AFTER | REMOVE_RANGE);
        return it;
    }

    void clear()
    {
        Observer::operator()(this->begin(), BEFORE | RESET);
        _traits::container::clear();
        Observer::operator()(this->end(), AFTER | RESET);
    }

    template <typename T>
    ObservableContainer<C, Observer> &operator=(T &&t)
    {
        Observer::operator()(this->begin(), BEFORE | RESET);
        _traits::container::operator=(std::forward<T>(t));
        Observer::operator()(this->end(), AFTER | RESET);
        return *this;
    }
};

}