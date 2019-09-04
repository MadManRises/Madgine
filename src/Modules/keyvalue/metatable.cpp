#include "../moduleslib.h"

#include "metatable.h"
#include "scopeiterator.h"

namespace Engine {

ScopeIterator MetaTable::find(const std::string &key, TypedScopePtr scope) const
{
    for (const std::pair<const char *, Accessor> *p = mMember; p->first; ++p) {
        if (key == p->first) {
            return { scope, p };
        }
    }
    return { scope, nullptr };
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
    throw 0;
}

bool MetaTable::isEditable(const std::string &key) const
{
    for (const std::pair<const char *, Accessor> *p = mMember; p->first; ++p) {
        if (key == p->first) {
            return p->second.mSetter;
        }
    }
    throw 0;
}

bool MetaTable::isDerivedFrom(const MetaTable *baseType) const
{
	//TODO inheritance
    return this == baseType;
}

}