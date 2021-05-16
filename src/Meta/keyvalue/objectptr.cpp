#include "../metalib.h"
#include "objectptr.h"
#include "objectinstance.h"

namespace Engine {
ObjectPtr::ObjectPtr()
{
}

ObjectPtr::ObjectPtr(const std::shared_ptr<ObjectInstance> &instance)
    : mInstance(instance)
{
}

void ObjectPtr::reset()
{
    mInstance.reset();
}

ObjectPtr::operator bool() const
{
    return mInstance.operator bool();
}

void ObjectPtr::setValue(const std::string_view &name, const ValueType &value)
{
    mInstance->setValue(name, value);
}

bool ObjectPtr::getValue(ValueType &retVal, const std::string_view &name) const
{
    if (!mInstance)
        return false;
    return mInstance->getValue(retVal, name);
}

bool ObjectPtr::operator==(const ObjectPtr &other) const
{
    return mInstance == other.mInstance;
}

}
