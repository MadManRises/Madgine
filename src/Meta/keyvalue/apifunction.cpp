#include "../metalib.h"

#include "apifunction.h"

#include "functiontable.h"

#include "Generic/execution/execution.h"

#include "valuetype.h"

namespace Engine {

ArgumentList ApiFunction::operator()(const ArgumentList &args) const
{
    return std::visit(overloaded {
                          [&](FunctionTable::FSyncPtr f) {
                              ArgumentList results;
                              f(mTable, results, args);
                              return results;
                          },
                          [&](FunctionTable::FAsyncPtr f) {
                              return static_cast<ArgumentList>(Execution::sync_expect(sender(args)));
                          } },
        mTable->mFunctionPtr);
}

void ApiFunction::operator()(KeyValueReceiver &receiver, const ArgumentList &args) const
{
    return std::visit(overloaded {
                          [&](FunctionTable::FSyncPtr f) {
                              ArgumentList results;
                              f(mTable, results, args);
                              receiver.set_value(std::move(results));
                          },
                          [&](FunctionTable::FAsyncPtr f) {
                              f(mTable, receiver, args);
                          } },
        mTable->mFunctionPtr);
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