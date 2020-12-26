#pragma once

#include "../generic/type_holder.h"
#include "Interfaces/util/stringutil.h"
#include "decay.h"

template <typename T>
extern const Engine::TypeInfo typeInfo;

#define RegisterType(T) template <> inline constexpr const ::Engine::TypeInfo typeInfo<T> = { #T, __FILE__, ::Engine::type_holder<T> };


namespace Engine {

template <typename T>
inline const char *typeName()
{
    return typeInfo<T>.mTypeName;
}

struct TypeInfo {
    static constexpr const char *fix(const char *s)
    {
        //TODO <> templates
        const char *f = strrchr(s, ':');
        return f ? f + 1 : s;
    }

    template <typename T>
    constexpr TypeInfo(const char *fullName, const char *headerPath, type_holder_t<T> type)
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
    const TypeInfo *mDecayType;
};


}
