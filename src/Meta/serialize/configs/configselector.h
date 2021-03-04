#pragma once

#include "Generic/type_pack.h"

namespace Engine {
namespace Serialize {

	template <typename Category>
    struct CategoryChecker {
		template <typename Config>
		using check = std::is_same<typename Config::Category, Category>;
	};

	template <typename Category, typename... Configs>
	using ConfigSelector = type_pack_unpack_unique_t<type_pack_filter_if_t<type_pack<Configs...>, CategoryChecker<Category>::template check>>; 

	template <typename Category, typename Default, typename... Configs>
	using ConfigSelectorDefault = type_pack_unpack_unique_t<type_pack_filter_if_t<type_pack<Configs...>, CategoryChecker<Category>::template check>, Default>; 

}
}