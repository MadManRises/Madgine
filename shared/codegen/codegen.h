#pragma once

#include "Modules/generic/bits.h"

namespace CodeGen {

struct MADGINE_CODEGEN_EXPORT File {



    void beginCondition(const std::string_view &cond);
    void endCondition(const std::string_view &cond);



    std::vector<size_t> mConditionalsStack;
    std::vector<std::string> mConditionalTokenList;

    Engine::BitArray<64> mConditionalsBitMask;
};

}