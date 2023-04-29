#pragma once

#include "../nodebase.h"
#include "../nodecollector.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT TestNode : Node<TestNode> {

        TestNode(NodeGraph &graph);
        TestNode(const TestNode &other, NodeGraph &graph);

        virtual size_t flowInCount() const override;

        virtual size_t flowOutCount() const override;

        virtual size_t dataInCount() const override;
        virtual std::string_view dataInName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataInType(uint32_t index, bool bidir = true) const override;

        virtual size_t dataOutCount() const override;
        virtual std::string_view dataOutName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataOutType(uint32_t index, bool bidir = true) const override;

        virtual void interpret(NodeReceiver receiver, uint32_t flowIn, std::unique_ptr<NodeInterpreterData> &data) const override;

        int mDummy;
    };

}
}

REGISTER_TYPE(Engine::NodeGraph::TestNode)
