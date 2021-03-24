#include "../metalib.h"

#include "function.h"

namespace Engine {

void Function::operator()(ValueType &retVal, const ArgumentList &args) const
{
    mWrapper(mFunction, retVal, args);
}

}