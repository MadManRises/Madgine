#include "../moduleslib.h"

#include "typedscopeptr.h"
#include "scopeiterator.h"
#include "valuetype.h"

namespace Engine {

    std::optional<ValueType> TypedScopePtr::get(const std::string &key) const
    {
        return mType->get(key, *this);
    }

	void TypedScopePtr::set(const std::string &key, const ValueType &value)
    {
        mType->set(key, value, *this);
    }

	bool TypedScopePtr::isEditable(const std::string &key) const
    {
        return mType->isEditable(key);
    }

    ScopeIterator TypedScopePtr::find(const std::string &key) const
    {
        return mType->find(key, *this);
    }

    ScopeIterator TypedScopePtr::begin() const
    {
        return { *this, mType->mMember };
    }

    ScopeIterator TypedScopePtr::end() const
    {        
        return { *this, nullptr};
    }

    std::string TypedScopePtr::name() const
    {
        if (mScope)
            return mType->name(*this);
        else
            return "<NULL>";
    }

}