#include "../moduleslib.h"

#include "metatable.h"
#include "scopeiterator.h"
#include "accessor.h"
#include "valuetype.h"

namespace Engine {

ScopeIterator MetaTable::find(const std::string &key, TypedScopePtr scope) const
{
    for (const std::pair<const char *, Accessor> *p = mMember; p->first; ++p) {
        if (key == p->first) {
            return { scope, p };
        }
    }
    if (mBaseGetter) {
        return mBaseGetter().find(key, scope);
    } else {
        return { scope, nullptr };
    }
}

std::optional<ValueType> MetaTable::get(const std::string &key, TypedScopePtr scope) const
{
    ScopeIterator it = find(key, scope);
    if (it != ScopeIterator { scope, nullptr })
        return (*it).second;
    else
        return {};
}

void MetaTable::set(const std::string &key, const ValueType &value, TypedScopePtr scope) const
{
    for (const std::pair<const char *, Accessor> *p = mMember; p->first; ++p) {
        if (key == p->first) {
            p->second.mSetter(scope, value);
            return;
        }
    }
    if (mBaseGetter) {
        mBaseGetter().set(key, value, scope);
    } else {
        std::terminate();
    }
}

bool MetaTable::isEditable(const std::string &key) const
{
    for (const std::pair<const char *, Accessor> *p = mMember; p->first; ++p) {
        if (key == p->first) {
            return p->second.mSetter;
        }
    }
    if (mBaseGetter) {
        return mBaseGetter().isEditable(key);
    } else {
        std::terminate();
    }
}

bool MetaTable::isDerivedFrom(const MetaTable *baseType) const
{
    return this == baseType || (mBaseGetter && mBaseGetter().isDerivedFrom(baseType));
}

std::string MetaTable::name(TypedScopePtr scope) const
{
    std::optional<ValueType> name = get("Name", scope);
    if (name && name->is<std::string>()) {
        return name->as<std::string>();
    } else {
        return "<"s + mTypeName + ">";
    }
}
}