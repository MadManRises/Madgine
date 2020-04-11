#pragma once

#include "Modules/keyvalue/valuetype.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT TargetPin {
        GraphExecutable *mNode = nullptr;
        size_t mIndex = 0;

        operator bool() const;
    };

    struct FlowOutPinPrototype {
        TargetPin mTarget;
    };

    struct DataInPinPrototype {
        TargetPin mSource;
        ValueType mDefaultValue;
    };

    struct DataOutPinPrototype {
        ValueType::ExtendedTypeDesc mType;
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
        size_t mIndex;
    };

}
}

MAKE_TUPLEFYABLE(Engine::NodeGraph::DataInPinPrototype)