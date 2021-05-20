#include "../baselib.h"

#include "keyvalueregistry.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {

std::map<std::string_view, TypedScopePtr> sGlobalRegistry;
Threading::WorkgroupLocal<std::map<std::string_view, TypedScopePtr>> sWorkGroupLocalRegistry;

void KeyValueRegistry::registerGlobal(const char *name, TypedScopePtr ptr)
{
    auto pib = sGlobalRegistry.try_emplace(name, ptr);
    assert(pib.second);
}

void KeyValueRegistry::registerWorkGroupLocal(const char *name, TypedScopePtr ptr)
{
    auto pib = sWorkGroupLocalRegistry->try_emplace(name, ptr);
    assert(pib.second);
}

void KeyValueRegistry::unregisterGlobal(TypedScopePtr ptr)
{
    auto it = std::find_if(sGlobalRegistry.begin(), sGlobalRegistry.end(), [=](auto &p) { return p.second == ptr; });
    sGlobalRegistry.erase(it);
}

void KeyValueRegistry::unregisterWorkGroupLocal(TypedScopePtr ptr)
{
    auto it = std::find_if(sWorkGroupLocalRegistry->begin(), sWorkGroupLocalRegistry->end(), [=](auto &p) { return p.second == ptr; });
    sWorkGroupLocalRegistry->erase(it);
}

const std::map<std::string_view, TypedScopePtr> &KeyValueRegistry::globals()
{
    return sGlobalRegistry;
}

const std::map<std::string_view, TypedScopePtr> &KeyValueRegistry::workgroupLocals()
{
    return sWorkGroupLocalRegistry;
}

}