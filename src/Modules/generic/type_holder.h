#pragma once

namespace Engine {

	
template <typename>
struct type_holder_t {
};

template <typename T>
const constexpr type_holder_t<T> type_holder = {};


}