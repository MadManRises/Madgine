#include "../metalib.h"

#include "boundapifunction.h"
#include "valuetype.h"

namespace Engine {

void BoundApiFunction::operator()(ValueType &retVal, const ArgumentList &args) const
{
    ArgumentList fullArgs;
    fullArgs.reserve(args.size() + 1);
    fullArgs.push_back(ValueType { mScope });
    fullArgs.insert(fullArgs.end(), args.begin(), args.end());
    mMethod(retVal, fullArgs);
}

}