#include "../metalib.h"

#include "accessor.h"
#include "metatable.h"
#include "scopefield.h"
#include "scopeiterator.h"
#include "valuetype.h"

namespace Engine {

ScopeIterator MetaTable::find(const std::string &key, TypedScopePtr scope) const
{
    for (const std::pair<const char *, Accessor> *p = mMember; p->first; ++p) {
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
    ArgumentList actualArgs = args;
    actualArgs.emplace(actualArgs.begin(), scope);
    op->value().as<ApiFunction>()(retVal, actualArgs);
}

bool MetaTable::isDerivedFrom(const MetaTable *baseType, size_t *offset) const
{
    if (this == baseType)
        return true;
    assert(!offset);
    return mBase && (*mBase)->isDerivedFrom(baseType);
}

std::string MetaTable::name(TypedScopePtr scope) const
{
    ScopeIterator name = find("Name", scope);
    if (name != scope.end() && name->value().is<std::string>()) {
        return name->value().as<std::string>();
    } else {
        ScopeIterator proxy = find("__proxy", scope);
        if (proxy != scope.end() && proxy->value().is<TypedScopePtr>()) {
            return proxy->value().as<TypedScopePtr>().name();
        }
        return "<"s + mTypeName + ">";
    }
}
}