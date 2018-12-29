#pragma once



#ifdef _WIN32
#	ifdef __GNUC__
#		define DLL_EXPORT __attribute__((dllexport))
#	else
#		define DLL_EXPORT __declspec(dllexport)
#	endif
#else
#	define DLL_EXPORT __attribute__((visibility("default")))
#endif

#ifdef _WIN32
#	ifdef __GNUC__
#		define DLL_IMPORT __attribute__((dllimport))
#	else
#		define DLL_IMPORT __declspec(dllimport)
#	endif
#else
#	define DLL_IMPORT __attribute__((weak))
#endif

