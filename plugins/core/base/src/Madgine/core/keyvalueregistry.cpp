#include "../baselib.h"

#include "keyvalueregistry.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {

std::map<TypedScopePtr, const char *> sGlobalRegistry;
Threading::WorkgroupLocal<std::map<TypedScopePtr, const char *>> sWorkGroupLocalRegistry;

void KeyValueRegistry::registerGlobal(const char *name, TypedScopePtr ptr)
{
    auto pib = sGlobalRegistry.try_emplace(ptr, name);
    assert(pib.second);
}

void KeyValueRegistry::registerWorkGroupLocal(const char *name, TypedScopePtr ptr)
{
    auto pib = sWorkGroupLocalRegistry->try_emplace(ptr, name);
    assert(pib.second);
}

void KeyValueRegistry::unregisterGlobal(TypedScopePtr ptr)
{
    size_t count = sGlobalRegistry.erase(ptr);
    assert(count == 1);
}

void KeyValueRegistry::unregisterWorkGroupLocal(TypedScopePtr ptr)
{
    size_t count = sWorkGroupLocalRegistry->erase(ptr);
    assert(count == 1);
}

const std::map<TypedScopePtr, const char *> &KeyValueRegistry::globals()
{
    return sGlobalRegistry;
}

const std::map<TypedScopePtr, const char *> &KeyValueRegistry::workgroupLocals()
{
    return sWorkGroupLocalRegistry;
}

}