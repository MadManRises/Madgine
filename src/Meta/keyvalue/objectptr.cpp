#include "../metalib.h"
#include "objectptr.h"
#include "objectinstance.h"

namespace Engine {

ObjectPtr::ObjectPtr(std::monostate)
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

ObjectInstance *ObjectPtr::get()
{
    return mInstance.get();
}

const ObjectInstance *ObjectPtr::get() const
{
    return mInstance.get();
}

ObjectPtr::operator bool() const
{
    return mInstance.operator bool();
}

void ObjectPtr::setValue(std::string_view name, const ValueType &value)
{
    mInstance->setValue(name, value);
}

bool ObjectPtr::getValue(ValueType &retVal, std::string_view name) const
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
