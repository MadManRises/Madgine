#pragma once

#include "container_traits.h"
#include "observerevent.h"

namespace Engine {
namespace {

    template <typename _traits, typename Observer>
    struct ObservableContainerImpl : _traits::container, private Observer {
        struct traits : _traits {
            typedef ObservableContainerImpl<_traits, Observer> container;

            typedef typename _traits::iterator iterator;
            typedef typename _traits::const_iterator const_iterator;

            template <class... _Ty>
            static std::pair<iterator, bool> emplace(container &c, const iterator &where, _Ty &&... args)
            {
                return c.emplace(where, std::forward<_Ty>(args)...);
            }
        };

        typedef typename traits::iterator iterator;
        typedef typename traits::const_iterator const_iterator;

        template <class... _Ty>
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
        ObservableContainerImpl<_traits, Observer> &operator=(T &&t)
        {
            Observer::operator()(this->begin(), BEFORE | RESET);
            _traits::container::operator=(std::forward<T>(t));
            Observer::operator()(this->end(), AFTER | RESET);
            return *this;
        }
    };

    template <template <typename...> typename C, typename Observer>
    struct PartialObservableContainer {
        template <typename T>
        using type = ObservableContainerImpl<container_traits<C<T>>, Observer>;
    };

}

/*template <template <typename...> typename C, typename Observer>
	using PartialObservableContainer = PartialObservableContainerHelper<C, Observer>::type;*/

template <typename C, typename Observer>
using ObservableContainer = ObservableContainerImpl<container_traits<C>, Observer>;

}