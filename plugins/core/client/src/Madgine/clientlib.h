#pragma once

/// @cond

#if defined(Client_EXPORTS)
#    define MADGINE_CLIENT_EXPORT DLL_EXPORT
#else
#    define MADGINE_CLIENT_EXPORT DLL_IMPORT
#endif

#include "Madgine/baselib.h"

#include <array>
#include <future>
#include <map>
#include <memory>
#include <sstream>
#include <stack>
#include <typeindex>
#include <vector>

#include "clientforward.h"

/*#if !defined(_HAS_TR1_NAMESPACE)
#include <unordered_map>
namespace std {
	namespace tr1 {
		template <class Key>
		using hash = std::hash<Key>;

		template <class Key, class T>
		using unordered_map = std::unordered_map<Key, T>;
	}
}
#endif*/

/// @endcond
