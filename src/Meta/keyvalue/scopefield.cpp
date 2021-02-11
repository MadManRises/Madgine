#include "../metalib.h"

#include "scopefield.h"
#include "typedscopeptr.h"

#include "valuetype.h"

#include "accessor.h"

namespace Engine {

ScopeField::ScopeField(const TypedScopePtr &ptr, const std::pair<const char *, Accessor> *pointer)
    : mScope(ptr)
    , mPointer(pointer)
{
}

ValueType ScopeField::value() const
{
    ValueType result; 
    mPointer->second.mGetter(result, mScope);
    return result;
}

ScopeField::operator ValueType() const {
    return value();
}

ScopeField &ScopeField::operator=(const ValueType &v)
{
    mPointer->second.mSetter(mScope, v);    
    return *this;
}

const char* ScopeField::key() const {
    return mPointer->first;
}

bool ScopeField::isEditable() const
{
    return mPointer->second.mSetter;
}

bool ScopeField::isGeneric() const
{
    return mPointer->second.mIsGeneric;
}

}