#include "../moduleslib.h"

#include "valuetype_desc.h"

namespace Engine {

std::string_view ValueTypeIndex::toString() const
{
    switch (mIndex) {
#define VALUETYPE_SEP
#define VALUETYPE_TYPE(Name, Storage, ...) \
    case ValueTypeEnum::Name##Value:       \
        return #Name;
#include "valuetypedef.h"
    default:
        std::terminate();
    }
}

bool ValueTypeDesc::canAccept(const ValueTypeDesc &valueType)
{
    if (mType != valueType.mType)
        return false;
    switch (mType) {
    case ValueTypeEnum::ScopeValue:
        if (!mSecondary.mMetaTable)
            return true;
        return (*valueType.mSecondary.mMetaTable)->isDerivedFrom(*mSecondary.mMetaTable);
    default:
        return true;
    }
}

std::string ValueTypeDesc::toString() const
{
    switch (mType) {
    case ValueTypeEnum::ScopeValue:
        return std::string { (*mSecondary.mMetaTable)->mTypeName } + "*";
    default:
        return std::string { mType.toString() };
    }
}

std::string_view ExtendedValueTypeIndex::toString() const
{
    if (isRegular()) {
        return mType.toString();
    } else {
        switch (mExtendedType) {
        case ExtendedValueTypeEnum::GenericType:
            return "Engine::ValueType";
        default:
            throw 0;
        }
    }
}

std::string ExtendedValueTypeDesc::toString() const
{
    if (mType.isRegular()) {
        return static_cast<ValueTypeDesc>(*this).toString();
    } else {
        return std::string { mType.toString() };
    }
}


}