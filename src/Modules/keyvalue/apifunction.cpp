#include "../moduleslib.h"

#include "apifunction.h"

#include "functiontable.h"

namespace Engine {

void ApiFunction::operator()(ValueType &retVal, const ArgumentList &args) const
{
    mTable->mFunctionPtr(mTable, retVal, args);
}

size_t ApiFunction::argumentsCount(bool excludeThis) const
{
    return mTable->mArgumentsCount - (excludeThis && mTable->mIsMemberFunction);
}

bool ApiFunction::isMemberFunction() const
{
    return mTable->mIsMemberFunction;
}

}