#pragma once

#include "macros.h"

#ifdef _WIN32
#ifdef __GNUC__
#define DLL_EXPORT_TAG __attribute__((dllexport))
#define DLL_IMPORT_TAG __attribute__((dllimport))
#else
#define DLL_EXPORT_TAG __declspec(dllexport)
#define DLL_IMPORT_TAG __declspec(dllimport)
#endif
#else
#define DLL_EXPORT_TAG __attribute__((visibility("default")))
#define DLL_IMPORT_TAG __attribute__((weak))
#endif

#if defined(STATIC_BUILD)

#define DLL_IMPORT
#define DLL_EXPORT

#else
#define DLL_IMPORT DLL_IMPORT_TAG
#define DLL_EXPORT DLL_EXPORT_TAG

#endif

/*
//+ clang on windows
//- MSVC
#define DLL_IMPORT_VARIABLE2(Type, Name, ...) template <__VA_ARGS__> DLL_IMPORT_VARIABLE Type Name;
#define DLL_EXPORT_VARIABLE2(Type, ns, Name, Init, ...) template <> \
                                              DLL_EXPORT_VARIABLE Type (ns Name<__VA_ARGS__>) = Init;
*/

#define DLL_IMPORT_VARIABLE2(Type, Name, ...) \
    template <__VA_ARGS__>                    \
    Type &Name();

#define DLL_EXPORT_VARIABLE2(qualifier, Type, ns, Name, Init, ...)       \
    static qualifier Type CONCAT2(__##Name##_helper_, __LINE__) = Init; \
    template <>                                               \
    DLL_EXPORT Type &ns Name<__VA_ARGS__>()                   \
    {                                                         \
        return CONCAT2(__##Name##_helper_, __LINE__);         \
    }                                                         \
    template Type &ns Name<__VA_ARGS__>();

#define TEMPLATE_INSTANTIATION template
#define TEMPLATE_INSTANTIATION_EXPORT template
#define TEMPLATE_INSTANTIATION_IMPORT extern template

#if _WIN32
#define WINDOWS 1
#else
#define WINDOWS 0
#endif

#if __unix__
#define UNIX 1
#if __EMSCRIPTEN__
#define EMSCRIPTEN 1
#else
#define EMSCRIPTEN 0
#endif
#if __linux__
#if __ANDROID__
#define ANDROID 1
#define LINUX 0
#else
#define LINUX 1
#define ANDROID 0
#endif
#endif
#else
#define UNIX 0
#define ANDROID 0
#define LINUX 0
#define EMSCRIPTEN 0
#endif
