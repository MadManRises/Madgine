#pragma once

#include "../nodeprototypecollector.h"
#include "../nodeprototypebase.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT TestNode : NodePrototype<TestNode> {

        TestNode();

        virtual size_t flowInCount() const override;
        virtual std::string_view flowInName(uint32_t index) const override;

        virtual std::string_view flowOutName(uint32_t index) const override;

        virtual std::string_view dataInName(uint32_t index) const override;
        virtual ExtendedValueTypeDesc dataInExpectedType(uint32_t index) const override;

        virtual std::string_view dataOutName(uint32_t index) const override;

        virtual void execute(NodeInstance *instance, uint32_t flowIndex, ArgumentList *out = nullptr) const override;
    };

}
}

RegisterType(Engine::NodeGraph::TestNode);