#pragma once

#include "Interfaces/stringutil.h"
#include "Interfaces/macros.h"
#include "decay.h"

namespace Engine {

	template <typename T>
struct typeMarker_t {
};

template <typename T>
constexpr typeMarker_t<T> typeMarker {};


}

DLL_IMPORT_VARIABLE2(const Engine::TypeInfo, typeInfo, typename T);

#define RegisterType(T) DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::TypeInfo, ::, typeInfo, SINGLE_ARG3({ #T, __FILE__, ::Engine::typeMarker<T> }), T)


namespace Engine {
/*
template <class T>
struct AccessClassInfo {
    static const TypeInfo &get()
    {
        return typeInfoInstance<T>();
    }
};

template <class T>
inline const TypeInfo &typeInfo()
{
    return AccessClassInfo<T>::get();
}
*/
template <class T>
inline const char *typeName()
{
    return typeInfo<T>().mTypeName;
}

struct TypeInfo {
    static constexpr const char *fix(const char *s)
    {
        //TODO <> templates
        const char *f = strrchr(s, ':');
        return f ? f + 1 : s;
    }

    template <typename T>
    constexpr TypeInfo(const char *fullName, const char *headerPath, typeMarker_t<T> type)
        : mFullName(fullName)
        , mTypeName(fix(mFullName))
        , mHeaderPath(headerPath)
        , mDecayType(std::is_same_v<decay_t<T>, void> ? nullptr : &typeInfo<decay_t<T>>)
    {
    }

    inline std::string namespaceName() const
    {
        return std::string { mFullName, strlen(mFullName) - 2 - strlen(mTypeName) };
    }

    const char *mFullName;
    const char *mTypeName;
    const char *mHeaderPath;
    const TypeInfo &(*mDecayType)();
};


}
