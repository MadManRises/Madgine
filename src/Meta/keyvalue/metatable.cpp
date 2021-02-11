#include "../metalib.h"

#include "metatable.h"
#include "scopeiterator.h"
#include "accessor.h"
#include "valuetype.h"
#include "scopefield.h"

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

/*std::optional<ValueType> MetaTable::get(const std::string &key, TypedScopePtr scope) const
{
    for (const std::pair<const char *, Accessor> *p = mMember; p->first; ++p) {
        if (key == p->first) {
            return p->second.mGetter(scope);            
        }
    }
    if (mBaseGetter) {
        return mBaseGetter().get(key, scope);
    } else {
        return {};
    }
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
            return p->second.mSetter != nullptr;
        }
    }
    if (mBaseGetter) {
        return mBaseGetter().isEditable(key);
    } else {
        std::terminate();
    }
}*/

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
    if (name != ScopeIterator{ scope, nullptr } && (*name).value().is<std::string>()) {
        return (*name).value().as<std::string>();
    } else {
        return "<"s + mTypeName + ">";
    }
}
}