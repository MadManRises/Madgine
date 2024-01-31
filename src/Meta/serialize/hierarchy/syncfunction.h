#pragma once

#include "Generic/closure.h"

namespace Engine {
namespace Serialize {

    struct SyncFunction {
        void (*mWriteFunctionArguments)(const std::set<std::reference_wrapper<FormattedBufferedStream>, CompareStreamId> &, const void *);
        void (*mWriteFunctionResult)(FormattedBufferedStream &, const void *);
        StreamResult (*mReadFunctionAction)(SyncableUnitBase *, FormattedBufferedStream &, uint16_t, FunctionType, PendingRequest &);
        StreamResult (*mReadFunctionRequest)(SyncableUnitBase *, FormattedBufferedStream &, uint16_t, FunctionType, MessageId);
    };

}
}