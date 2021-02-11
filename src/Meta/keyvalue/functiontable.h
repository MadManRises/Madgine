#pragma once

#include "valuetype_desc.h"

namespace Engine {

struct FunctionArgument {
    ExtendedValueTypeDesc mType;
    std::string_view mName;
};

struct FunctionTable {
    typedef void (*FPtr)(const FunctionTable *, ValueType &, const ArgumentList &);
    FPtr mFunctionPtr;

    size_t mArgumentsCount;
    bool mIsMemberFunction;
    const FunctionArgument *mArguments;
};

}

DLL_IMPORT_VARIABLE2(const Engine::FunctionTable, function, auto F);