#pragma once



#ifdef _WIN32
#	ifdef __GNUC__
#		define DLL_EXPORT __attribute__((dllexport))
#		define DLL_IMPORT __attribute__((dllimport))
#	else
#		define DLL_EXPORT __declspec(dllexport)
#		define DLL_IMPORT __declspec(dllimport)
#	endif
#else
#	define DLL_EXPORT __attribute__((visibility("default")))
#	define DLL_IMPORT __attribute__((weak))
#endif

#define TEMPLATE_INSTANTIATION template
#define TEMPLATE_INSTANTIATION_EXPORT template
#define TEMPLATE_INSTANTIATION_IMPORT extern template