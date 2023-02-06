#include "../rootlib.h"

#include "keyvalueregistry.h"

#include "Modules/threading/workgroupstorage.h"

#include "Generic/projections.h"

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
    auto it = std::ranges::find(sGlobalRegistry, ptr, projectionPairSecond);
    sGlobalRegistry.erase(it);
}

void KeyValueRegistry::unregisterWorkGroupLocal(TypedScopePtr ptr)
{
    auto it = std::ranges::find(*sWorkGroupLocalRegistry, ptr, projectionPairSecond);
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