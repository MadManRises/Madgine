#pragma once

namespace Engine {

struct FunctionTable {
    typedef void (*FPtr)(const FunctionTable *, ValueType &, const ArgumentList &);
    FPtr mFunctionPtr;

    size_t mArgumentsCount;
    bool mIsMemberFunction;
    const FunctionArgument *mArguments;
};

}

DLL_IMPORT_VARIABLE2(const Engine::FunctionTable, function, auto F);