#pragma once

#include "keyvaluereceiver.h"

namespace Engine {

struct META_EXPORT ApiFunction {    

    constexpr ApiFunction() = default;

    constexpr ApiFunction(const FunctionTable *table)
        : mTable(table)
    {
    }

    bool operator==(const ApiFunction &other) const
    {
        return mTable == other.mTable;
    }

    explicit operator bool() const
    {
        return mTable != nullptr;
    }

    ArgumentList operator()(const ArgumentList &args) const;
    auto sender(const ArgumentList& args) const {
        return make_key_value_sender(*this, args);
    }
    void operator()(KeyValueReceiver &receiver, const ArgumentList &args) const;

    size_t argumentsCount(bool excludeThis = false) const;
    bool isMemberFunction() const;
    
    const FunctionTable *mTable = nullptr;
};

}