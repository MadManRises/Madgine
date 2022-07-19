#pragma once

#include "Generic/bits.h"

namespace CodeGen {

struct MADGINE_CODEGEN_EXPORT File {

    void beginCondition(std::string_view cond);
    void endCondition(std::string_view cond);

    virtual void statement(Statement statement) = 0;

    std::vector<size_t> mConditionalsStack;
    std::vector<std::string> mConditionalTokenList;

    Engine::BitArray<64> mConditionalsBitMask;
};

}