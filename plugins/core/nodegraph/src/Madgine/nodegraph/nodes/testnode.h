#pragma once

#include "../nodeprototypecollector.h"
#include "../nodeprototypebase.h"

namespace Engine {
namespace NodeGraph {

    struct MADGINE_NODEGRAPH_EXPORT TestNode : NodePrototype<TestNode> {

        TestNode();

        virtual size_t flowInCount() const override;
        virtual std::string_view flowInName(size_t index) const override;

        virtual std::string_view flowOutName(size_t index) const override;

        virtual std::string_view dataInName(size_t index) const override;
        virtual ExtendedValueTypeDesc dataInExpectedType(size_t index) const override;

        virtual std::string_view dataOutName(size_t index) const override;

        virtual void execute(NodeInstance *instance, size_t flowIndex, ArgumentList *out = nullptr) const override;
    };

}
}

RegisterType(Engine::NodeGraph::TestNode);