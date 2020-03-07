#include "../moduleslib.h"

#include "boundapimethod.h"
#include "valuetype.h"

namespace Engine {

void BoundApiMethod::operator()(ValueType &retVal, const ArgumentList &args) const
{
    return mMethod(retVal, mScope, args);
}

}