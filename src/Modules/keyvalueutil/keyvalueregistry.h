#pragma once

#include "../keyvalue/typedscopeptr.h"
#include "../threading/workgroupstorage.h"

namespace Engine {

struct MODULES_EXPORT KeyValueRegistry {
    static void registerGlobal(const char *name, TypedScopePtr ptr);
    static void registerWorkGroupLocal(const char *name, TypedScopePtr ptr);
    static void unregisterGlobal(TypedScopePtr ptr);
    static void unregisterWorkGroupLocal(TypedScopePtr ptr);

    static const std::map<TypedScopePtr, const char *> &globals();
    static const std::map<TypedScopePtr, const char *> &workgroupLocals();
};

template <typename T>
struct KeyValueWorkGroupLocal : T {

    using decay_t = T;

    template <typename... Args>
    KeyValueWorkGroupLocal(const char *name, Args &&... args)
        : T(std::forward<Args>(args)...)
    {
        KeyValueRegistry::registerWorkGroupLocal(name, this);
    }

    ~KeyValueWorkGroupLocal()
    {
        KeyValueRegistry::unregisterWorkGroupLocal(this);
    }
};

template <typename T>
struct KeyValueGlobal : T {

    using decay_t = T;

    template <typename... Args>
    KeyValueGlobal(const char *name, Args &&... args)
        : T(std::forward<Args>(args)...)
    {
        KeyValueRegistry::registerGlobal(name, this);
    }

    ~KeyValueGlobal()
    {
        KeyValueRegistry::unregisterGlobal(this);
    }
};

}