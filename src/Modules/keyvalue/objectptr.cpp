#include "../moduleslib.h"
#include "objectptr.h"
#include "objectfieldaccessor.h"
#include "objectinstance.h"
#include "valuetype.h"

namespace Engine {
ObjectPtr::ObjectPtr()
{
}

ObjectPtr::ObjectPtr(const std::shared_ptr<ObjectInstance> &instance)
    : mInstance(instance)
{
}

void ObjectPtr::clear()
{
    mInstance.reset();
}

ObjectFieldAccessor ObjectPtr::operator[](const std::string &name)
{
    return ObjectFieldAccessor(mInstance, name);
}

ObjectFieldAccessor ObjectPtr::operator[](const char *name)
{
    return ObjectFieldAccessor(mInstance, name);
}

ValueType ObjectPtr::operator[](const std::string &name) const
{
    return getValue(name);
}

ValueType ObjectPtr::operator[](const char *name) const
{
    return getValue(name);
}

ObjectPtr::operator bool() const
{
    return mInstance.operator bool();
}

void ObjectPtr::setValue(const std::string &name, const ValueType &value)
{
    mInstance->setValue(name, value);
}

ValueType ObjectPtr::getValue(const std::string &name) const
{
    return mInstance ? mInstance->getValue(name) : ValueType();
}

bool ObjectPtr::operator==(const ObjectPtr &other) const
{
    return mInstance == other.mInstance;
}

}
