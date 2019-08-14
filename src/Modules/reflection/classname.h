#pragma once

#include "Interfaces/stringutil.h"
#include "Interfaces/macros.h"

namespace Engine {


struct TypeInfo {
    static constexpr const char *fix(const char *s)
    {
        //TODO <> templates
        const char *f = strrchr(s, ':');
        return f ? f + 1 : s;
    }

    constexpr TypeInfo(const char *fullName, const char *headerPath, const TypeInfo *decayType = nullptr)
        : mFullName(fullName)
        , mTypeName(fix(mFullName))
        , mHeaderPath(headerPath)
        , mDecayType(decayType)
    {
    }

    inline std::string namespaceName() const
    {
        return std::string { mFullName, strlen(mFullName) - 2 - strlen(mTypeName) };
    }

    const char *mFullName;
    const char *mTypeName;
    const char *mHeaderPath;
    const TypeInfo *mDecayType;
};

}

DLL_IMPORT_VARIABLE2(const Engine::TypeInfo, typeInfoInstance, typename T);

#define RegisterType(T) DLL_EXPORT_VARIABLE2(constexpr, const ::Engine::TypeInfo, ::, typeInfoInstance, SINGLE_ARG2({ #T, __FILE__ }), T)


namespace Engine {

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

template <class T>
inline const char *typeName()
{
    return typeInfo<T>().mTypeName;
}

}
