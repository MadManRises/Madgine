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
            for (const FlowOutPinPrototype& pin : mFlowOutPins) {
                if (pin.mTarget) {
                    mask &= mGraph.flowInMask(pin.mTarget, false);
                }
            }
            for (const DataInPinPrototype &pin : mDataInPins) {
                if (pin.mSource) {
                    mask &= mGraph.dataProviderMask(pin.mSource, false);
                }
            }
            for (const DataOutPinPrototype &pin : mDataOutPins) {
                if (pin.mTarget) {
                    mask &= mGraph.dataReceiverMask(pin.mTarget, false);
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
            for (const FlowInPinPrototype &pin : mFlowInPins) {
                for (Pin source : pin.mSources) {
                    mask &= mGraph.flowOutMask(source, false);
                }
            }
            for (const DataReceiverPinPrototype &pin : mDataReceiverPins) {
                for (Pin source : pin.mSources) {
                    mask &= mGraph.dataOutMask(source, false);
                }
            }
            for (const DataProviderPinPrototype &pin : mDataProviderPins) {
                for (Pin target : pin.mTargets) {
                    mask &= mGraph.dataInMask(target, false);
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