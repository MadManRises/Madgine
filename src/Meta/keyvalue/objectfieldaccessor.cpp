#include "../metalib.h"
#include "objectfieldaccessor.h"
#include "objectinstance.h"
#include "valuetype.h"

namespace Engine {

ObjectFieldAccessor::ObjectFieldAccessor(const std::shared_ptr<ObjectInstance> &instance, const std::string &name)
    : mInstance(instance)
    , mName(name)
{
}

void ObjectFieldAccessor::operator=(const ValueType &v)
{
    mInstance->setValue(mName, v);
}

ObjectFieldAccessor::operator ValueType()
{
    return mInstance ? mInstance->getValue(mName) : ValueType {};
}

}
