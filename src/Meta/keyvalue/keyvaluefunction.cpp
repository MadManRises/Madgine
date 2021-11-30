#include "../metalib.h"

#include "keyvaluefunction.h"

namespace Engine {

void KeyValueFunction::operator()(ValueType &retVal, const ArgumentList &args) const
{
    mWrapper(mFunction, retVal, args);
}

}