#pragma once

#include "../templates.h"


namespace Engine {

template <typename C>
struct underlying_container;

template <typename C, typename = void>
struct base_container {
    typedef C type;
};

template <typename C>
struct base_container<C, void_t<typename underlying_container<C>::type>> : base_container<typename underlying_container<C>::type> {
};


}