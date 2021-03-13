#pragma once

#include "Meta/keyvalue/valuetype.h"
#include "Generic/indextype.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT TargetPin {
        IndexType<uint32_t, 0> mNode;
        IndexType<uint32_t> mIndex;

        explicit operator bool() const;
    };

    struct FlowOutPinPrototype {
        TargetPin mTarget;
    };

    struct DataInPinPrototype {
        TargetPin mSource;
    };

    struct DataOutPinPrototype {
        TargetPin mTarget;
    };

    struct DataReceiverPinPrototype {
        ExtendedValueTypeDesc mType;
    };

    struct DataProviderPinPrototype {
        ExtendedValueTypeDesc mType;
    };

    enum class PinDir {
        In,
        Out
    };

    enum class PinType {
        Flow,
        Data,
        DataInstance
    };

    struct PinDesc {
        PinDir mDir;
        PinType mType;
        uint32_t mIndex;

        bool isCompatible(const PinDesc& other) const {
            if (mDir == other.mDir)
                return false;
            if (mType == PinType::Flow || other.mType == PinType::Flow)
                return mType == other.mType;
            return mType != other.mType;
        }
    };

}
}

