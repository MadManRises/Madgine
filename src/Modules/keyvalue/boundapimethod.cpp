#include "../moduleslib.h"

#include "boundapimethod.h"
#include "valuetype.h"

namespace Engine {

ValueType BoundApiMethod::operator()(const ArgumentList &args) const
{
    return mMethod(mScope, args);
}

}