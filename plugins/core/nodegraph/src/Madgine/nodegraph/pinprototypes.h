#pragma once

#include "Modules/keyvalue/valuetype.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT TargetPin {
        GraphExecutable *mNode = nullptr;
        uint32_t mIndex = 0;

        explicit operator bool() const;
    };

    struct FlowOutPinPrototype {
        TargetPin mTarget;
    };

    struct DataInPinPrototype {
        TargetPin mSource;
        ValueType mDefaultValue;
    };

    struct DataOutPinPrototype {
        ExtendedValueTypeDesc mType;
    };

    enum class PinDir {
        In,
        Out
    };

    enum class PinType {
        Flow,
        Data
    };

    struct PinDesc {
        PinDir mDir;
        PinType mType;
        uint32_t mIndex;
    };

}
}

MAKE_TUPLEFYABLE(Engine::NodeGraph::DataInPinPrototype, 2)
