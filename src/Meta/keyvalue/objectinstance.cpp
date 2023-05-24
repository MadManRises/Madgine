#include "../metalib.h"

#include "objectinstance.h"

#include "valuetype.h"

namespace Engine {

std::map<std::string_view, ValueType> ObjectInstance::values() const
{
    return {};
}

void ObjectInstance::call(KeyValueReceiver &receiver, const ArgumentList &args)

{
    throw 0;
}

std::string ObjectInstance::descriptor() const
{
    return "<object>";
}

}