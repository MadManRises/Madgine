#include "codegenlib.h"

#include "codegen.h"

namespace CodeGen {

void File::beginCondition(std::string_view cond)
{
    auto it = std::find(mConditionalTokenList.begin(), mConditionalTokenList.end(), cond);
    if (it == mConditionalTokenList.end()) {
        mConditionalTokenList.emplace_back(cond);
        it = std::prev(mConditionalTokenList.end());
        assert(mConditionalsStack.size() == std::distance(mConditionalTokenList.begin(), it));
        mConditionalsStack.emplace_back();
    }
    size_t index = std::distance(mConditionalTokenList.begin(), it);
    ++mConditionalsStack[index];
    if (mConditionalsStack[index] == 1)
        mConditionalsBitMask[index] = true;
}

void File::endCondition(std::string_view cond)
{
    auto it = std::find(mConditionalTokenList.begin(), mConditionalTokenList.end(), cond);
    assert(it != mConditionalTokenList.end());
    size_t index = std::distance(mConditionalTokenList.begin(), it);
    --mConditionalsStack[index];
    if (mConditionalsStack[index] == 0)
        mConditionalsBitMask[index] = false;
}

}