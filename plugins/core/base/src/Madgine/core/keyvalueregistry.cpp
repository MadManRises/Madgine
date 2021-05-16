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
    size_t count = std::erase_if(sGlobalRegistry, [=](auto &p) { return p.second == ptr; });
    assert(count == 1);
}

void KeyValueRegistry::unregisterWorkGroupLocal(TypedScopePtr ptr)
{
    size_t count = std::erase_if(*sWorkGroupLocalRegistry, [=](auto &p) { return p.second == ptr; });
    assert(count == 1);
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