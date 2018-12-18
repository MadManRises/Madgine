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

#ifndef STATIC_BUILD
#	ifdef PLUGIN_BUILD
#		define PLUGIN_LOCAL_IMPL(pre, post) pre ## _ ## post
#		define PLUGIN_LOCAL_EVAL(pre, post) PLUGIN_LOCAL_IMPL(pre, post)
#		define PLUGIN_LOCAL(name) PLUGIN_LOCAL_EVAL(name, PLUGIN_NAME)
#	else
#		define PLUGIN_LOCAL(name) name ## _Base
#	endif
#endif
