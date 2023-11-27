#include "../metalib.h"

#include "accessor.h"
#include "metatable.h"
#include "scopefield.h"
#include "scopeiterator.h"
#include "valuetype.h"

namespace Engine {

ScopeIterator MetaTable::find(std::string_view key, TypedScopePtr scope) const
{
    for (const std::pair<const char *, Accessor> *p = mMembers; p->first; ++p) {
        if (key == p->first) {
            return { scope, p };
        }
    }
    if (mBase) {
        return (*mBase)->find(key, scope);
    } else {
        return { scope, nullptr };
    }
}

void MetaTable::call(TypedScopePtr scope, ValueType &retVal, const ArgumentList &args) const
{
    ScopeIterator op = find("__call", scope);
    if (op == scope.end())
        throw "No call-operator for type!";
    ValueType f;
    op->value(f);
    f.as<BoundApiFunction>()(retVal, args);
}

bool MetaTable::isDerivedFrom(const MetaTable *baseType, size_t *offset) const
{
    if (this == baseType)
        return true;
    if (offset)
        *offset += mBaseOffset();
    return mBase && (*mBase)->isDerivedFrom(baseType, offset);
}

std::string MetaTable::name(TypedScopePtr scope) const
{
    ScopeIterator nameIt = find("Name", scope);
    if (nameIt != scope.end()) {
        ValueType name;
        nameIt->value(name);
        if (name.is<std::string>()) {
            return name.as<std::string>();
        }
    }
    ScopeIterator proxyIt = find("__proxy", scope);
    if (proxyIt != scope.end()){
        ValueType proxy;
        proxyIt->value(proxy);
        if (proxy.is<TypedScopePtr>()) {
            return proxy.as<TypedScopePtr>().name();
        }
    }
    return "<"s + mTypeName + ">";
}
}