#pragma once

/// @cond

#if defined(Widgets_EXPORTS)
#    define MADGINE_WIDGETS_EXPORT DLL_EXPORT
#else
#    define MADGINE_WIDGETS_EXPORT DLL_IMPORT
#endif

#include "Madgine/clientlib.h"


#include "widgetsforward.h"

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
