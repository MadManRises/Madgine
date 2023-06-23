#pragma once

#include "Generic/functor.h"

namespace Engine {

template <typename C, typename Observer = NoOpFunctor>
struct CompactContainer : C, Observer {

    typename C::iterator erase(const typename C::iterator& it) {
        auto last = --C::end();
        if (last != it) {
            using std::swap;
            swap(*it, *last);
            Observer::operator()(it, C::begin());
        }
        return C::erase(last);
    }

};

template <typename C, typename Observer>
struct underlying_container<CompactContainer<C, Observer>> {
    typedef C type;
};

template <typename C, typename Observer>
struct container_traits<CompactContainer<C, Observer>> : container_traits<C> {
    typedef CompactContainer<C, Observer> container;

    using _traits = container_traits<C>;

    typedef typename _traits::iterator iterator;
    typedef typename _traits::const_iterator const_iterator;

};

}