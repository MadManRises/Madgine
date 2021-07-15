#pragma once

#include "../../nodebase.h"
#include "../../nodegraph.h"

namespace Engine {
namespace NodeGraph {

    template <typename Base = NodeBase>
    struct AutoMaskNode : Base {
        using Base::Base;

        virtual uint32_t flowInMask(uint32_t index, bool bidir = true) const override
        {
            uint32_t mask = NodeExecutionMask::ALL;
            for (const FlowOutPinPrototype &pin : this->mFlowOutPins) {
                if (pin.mTarget) {
                    mask &= this->mGraph.flowInMask(pin.mTarget, false);
                }
            }
            for (const DataInPinPrototype &pin : this->mDataInPins) {
                if (pin.mSource) {
                    mask &= this->mGraph.dataProviderMask(pin.mSource, false);
                }
            }
            for (const DataOutPinPrototype &pin : this->mDataOutPins) {
                if (pin.mTarget) {
                    mask &= this->mGraph.dataReceiverMask(pin.mTarget, false);
                }
            }
            if (bidir) {
                mask &= flowOutMask(0, false);
            }
            return mask;
        }

        virtual uint32_t flowOutMask(uint32_t index, bool bidir = true) const override
        {
            uint32_t mask = NodeExecutionMask::ALL;
            for (const FlowInPinPrototype &pin : this->mFlowInPins) {
                for (Pin source : pin.mSources) {
                    mask &= this->mGraph.flowOutMask(source, false);
                }
            }
            for (const DataReceiverPinPrototype &pin : this->mDataReceiverPins) {
                for (Pin source : pin.mSources) {
                    mask &= this->mGraph.dataOutMask(source, false);
                }
            }
            for (const DataProviderPinPrototype &pin : this->mDataProviderPins) {
                for (Pin target : pin.mTargets) {
                    mask &= this->mGraph.dataInMask(target, false);
                }
            }
            if (bidir) {
                mask &= flowInMask(0, false);
            }
            return mask;
        }

        virtual uint32_t dataInMask(uint32_t index, bool bidir = true) const override
        {
            return flowOutMask(0, bidir);
        }

        virtual uint32_t dataOutMask(uint32_t index, bool bidir = true) const override
        {
            return flowOutMask(0, bidir);
        }

        virtual uint32_t dataReceiverMask(uint32_t index, bool bidir = true) const override
        {
            return flowInMask(0, bidir);
        }

        virtual uint32_t dataProviderMask(uint32_t index, bool bidir = true) const override
        {
            return flowInMask(0, bidir);
        }


    private:
    };

}

}