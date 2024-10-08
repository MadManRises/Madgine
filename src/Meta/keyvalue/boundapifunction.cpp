#include "../metalib.h"

#include "boundapifunction.h"
#include "valuetype.h"

#include "functionargument.h"

#include "functiontable.h"

namespace Engine {

BoundApiFunction::BoundApiFunction(const ApiFunction &f, const TypedScopePtr &scope)
    : mFunction(f)
    , mScope(scope.mScope)
{
    assert(f.mTable->mArguments[0].mType.mType == ValueTypeEnum::ScopeValue);
    assert(*f.mTable->mArguments[0].mType.mSecondary.mMetaTable == scope.mType);
}

TypedScopePtr BoundApiFunction::scope() const
{
    return { mScope, *mFunction.mTable->mArguments[0].mType.mSecondary.mMetaTable };
}

void BoundApiFunction::operator()(ValueType &retVal, const ArgumentList &args) const
{
    ArgumentList fullArgs;
    fullArgs.reserve(args.size() + 1);
    fullArgs.push_back(ValueType { scope() });
    fullArgs.insert(fullArgs.end(), args.begin(), args.end());
    mFunction(retVal, fullArgs);
}

}