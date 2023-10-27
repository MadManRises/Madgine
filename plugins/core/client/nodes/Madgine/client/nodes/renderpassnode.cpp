#include "../../clientnodeslib.h"

#include "renderpassnode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "shadercodegenerator.h"

#include "Madgine/nodegraph/nodegraph.h"

#include "Madgine/nodegraph/nodes/util/sendernode.h"

#include "Madgine/render/renderpass.h"

NODE(RenderPassNode, Engine::Render::RenderPassNode)

METATABLE_BEGIN_BASE(Engine::Render::RenderPassNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::Render::RenderPassNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Render::RenderPassNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::Render::RenderPassNode)

namespace Engine {
namespace Render {

    RenderPassNode::RenderPassNode(NodeGraph::NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    RenderPassNode::RenderPassNode(const RenderPassNode &other, NodeGraph::NodeGraph &graph)
        : Node(other, graph)
    {
    }

    size_t RenderPassNode::flowOutGroupCount() const
    {
        return 2;
    }

    size_t RenderPassNode::flowOutBaseCount(uint32_t group) const
    {
        return group == 1 ? 1 : 0;
    }

    std::string_view RenderPassNode::flowOutName(uint32_t index, uint32_t group) const
    {
        return "Render";
    }

    size_t RenderPassNode::dataProviderGroupCount() const
    {
        return 2;
    }

    size_t RenderPassNode::dataProviderBaseCount(uint32_t group) const
    {
        return 1;
    }

    std::string_view RenderPassNode::dataProviderName(uint32_t index, uint32_t group) const
    {
        return group == 0 ? "Pass" : "Target";
    }

    ExtendedValueTypeDesc RenderPassNode::dataProviderType(uint32_t index, uint32_t group, bool bidir) const
    {
        if (group == 0)
            return toValueTypeDesc<Render::RenderPass *>();
        else
            return toValueTypeDesc<Render::RenderTarget *>();
    }

    struct RenderPassNodeInterpret : NodeGraph::NodeInterpreterData, Render::RenderPass {

        RenderPassNodeInterpret(NodeGraph::NodeInterpreterState &interpreter, NodeGraph::Pin pin)
            : mInterpreter(interpreter)
            , mTargetPin(pin)
        {
        }

        virtual void render(Render::RenderTarget *target, size_t iteration) override
        {
            mTarget = target;
            mIteration = iteration;
            auto result = Execution::sync_expect(mInterpreter.interpret(mTargetPin));
        }

        virtual int priority() const override
        {
            return 50;
        }

        virtual std::string_view name() const override
        {
            return "NodeGraph";
        }

        NodeGraph::NodeInterpreterState &mInterpreter;
        NodeGraph::Pin mTargetPin;
        Render::RenderTarget *mTarget = nullptr;
        size_t mIteration;
    };

    void RenderPassNode::interpret(NodeGraph::NodeReceiver<NodeBase> receiver, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const
    {
        throw 0;
    }

    void RenderPassNode::interpretRead(NodeGraph::NodeInterpreterState &interpreter, ValueType &retVal, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t providerIndex, uint32_t group) const
    {
        if (!data) {
            data = std::make_unique<RenderPassNodeInterpret>(interpreter, flowOutTarget(0, 1));
        }
        RenderPassNodeInterpret *renderData = static_cast<RenderPassNodeInterpret *>(data.get());
        if (group == 0) {
            assert(providerIndex == 0);
            retVal = static_cast<Render::RenderPass *>(renderData);
        } else {
            assert(providerIndex == 0);
            retVal = renderData->mTarget;
        }
    }

}
}
