#include "../moduleslib.h"

#include "metatable.h"
#include "scopeiterator.h"

namespace Engine {

    ScopeIterator MetaTable::find(const std::string &key, ScopeBase *scope) const
    {
        for (const std::pair<const char *, Accessor> *p = mMember; p->first; ++p) {
            if (key == p->first) {
                return {scope, p};
            }
        }
        return { scope, nullptr };
    }

    std::optional<ValueType> MetaTable::get(const std::string &key, ScopeBase *scope) const
    {
        ScopeIterator it = find(key, scope);
        if (it != ScopeIterator{ scope, nullptr })
            return (*it).second;
        else
            return {};
    }

}