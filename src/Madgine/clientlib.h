#pragma once

/// @cond

#if defined(STATIC_BUILD)
#	define MADGINE_CLIENT_EXPORT
#	define MADGINE_CLIENT_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION
#else
#	if defined(Client_EXPORTS)
#		define MADGINE_CLIENT_EXPORT DLL_EXPORT
#		define MADGINE_CLIENT_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_EXPORT
#	else
#		define MADGINE_CLIENT_EXPORT DLL_IMPORT
#		define MADGINE_CLIENT_TEMPLATE_INSTANTIATION TEMPLATE_INSTANTIATION_IMPORT
#	endif
#endif

#include "baselib.h"

#include <stack>
#include <memory>
#include <map>
#include <array>
#include <sstream>
#include <vector>
#include <typeindex>
#include <future>

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


#ifdef _MSC_VER
#undef NO_ERROR
#endif

/// @endcond
