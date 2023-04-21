#include "../../clientnodeslib.h"

#include "rasterizernode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "shadercodegenerator.h"

#include "Madgine/nodegraph/nodegraph.h"

NODE(RasterizerNode, Engine::Render::RasterizerNode)

METATABLE_BEGIN_BASE(Engine::Render::RasterizerNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::Render::RasterizerNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Render::RasterizerNode, Engine::NodeGraph::NodeBase)
FIELD(mAdditionalPins)
SERIALIZETABLE_END(Engine::Render::RasterizerNode)

namespace Engine {
namespace Render {

    RasterizerNode::RasterizerNode(NodeGraph::NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    RasterizerNode::RasterizerNode(const RasterizerNode &other, NodeGraph::NodeGraph &graph)
        : Node(other, graph)
        , mAdditionalPins(other.mAdditionalPins)
    {
    }

    size_t RasterizerNode::flowInCount() const
    {
        return 1;
    }

    std::string_view RasterizerNode::flowInName(uint32_t index) const
    {
        return "vertex";
    }

    uint32_t RasterizerNode::flowInMask(uint32_t index, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    size_t RasterizerNode::flowOutCount() const
    {
        return 1;
    }

    std::string_view RasterizerNode::flowOutName(uint32_t index) const
    {
        return "pixel";
    }

    uint32_t RasterizerNode::flowOutMask(uint32_t index, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    size_t RasterizerNode::dataProviderCount() const
    {
        return dataInCount();
    }

    std::string_view RasterizerNode::dataProviderName(uint32_t index) const
    {
        return dataInName(index);
    }

    ExtendedValueTypeDesc RasterizerNode::dataProviderType(uint32_t index, bool bidir) const
    {
        if (index == 0)
            return toValueTypeDesc<Vector4>();
        if (index == mAdditionalPins + 1)
            return { ExtendedValueTypeEnum::GenericType };
        ExtendedValueTypeDesc desc = { ExtendedValueTypeEnum::GenericType };
        if (index <= mAdditionalPins) {
            NodeGraph::Pin pin = dataInSource(index);
            desc = mGraph.dataProviderType(pin, false);
        }
        if (bidir && desc.mType == ExtendedValueTypeEnum::GenericType)
            return dataInType(index, false);
        return desc;
    }

    uint32_t RasterizerNode::dataProviderMask(uint32_t index, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    size_t RasterizerNode::dataInCount() const
    {
        return 1 + mAdditionalPins;
    }

    std::string_view RasterizerNode::dataInName(uint32_t index) const
    {
        if (index == 0)
            return "pos";
        if (index <= mAdditionalPins) {
            NodeGraph::Pin pin = dataInSource(index);
            return mGraph.dataProviderName(pin);
        }
        if (index == mAdditionalPins + 1)
            return "<>";
        throw 0;
    }

    ExtendedValueTypeDesc RasterizerNode::dataInType(uint32_t index, bool bidir) const
    {
        if (index == 0)
            return toValueTypeDesc<Vector4>();
        if (index == mAdditionalPins + 1)
            return { ExtendedValueTypeEnum::GenericType };
        ExtendedValueTypeDesc desc = { ExtendedValueTypeEnum::GenericType };
        if (index <= mAdditionalPins) {
            if (!mDataProviderPins[index].mTargets.empty()) {
                NodeGraph::Pin pin = mDataProviderPins[index].mTargets.front();
                desc = mGraph.dataInType(pin, false);
            }
        }
        if (bidir && desc.mType == ExtendedValueTypeEnum::GenericType)
            return dataProviderType(index, false);
        return desc;
    }

    uint32_t RasterizerNode::dataInMask(uint32_t index, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    bool RasterizerNode::dataInVariadic() const
    {
        return true;
    }

    void RasterizerNode::onDataInUpdate(uint32_t index, NodeGraph::Pin source, NodeGraph::EdgeEvent event)
    {
        if (event == NodeGraph::CONNECT) {
            if (index == dataInCount()) {
                ++mAdditionalPins;
                setup();
            }
        } else if (event == NodeGraph::DISCONNECT) {
            removeDataInPin(index);
            removeDataProviderPin(index);

            --mAdditionalPins;
        }
    }

    void RasterizerNode::interpret(NodeGraph::NodeInterpreter &interpreter, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeGraph::NodeInterpreterData> &data) const
    {
        throw 0;
    }

    void RasterizerNode::generate(NodeGraph::CodeGenerator &_generator, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeGraph::CodeGeneratorData> &data) const
    {
        assert(flowInOut == 0);
        assert(!data);

        std::unique_ptr<ShaderCodeGeneratorData> _data = std::make_unique<ShaderCodeGeneratorData>();
        _data->mInstanceIndex = 1;
        data = std::move(_data);

        ShaderCodeGenerator &generator = dynamic_cast<ShaderCodeGenerator &>(_generator);

        CodeGen::ShaderFile &file = generator.mFile;

        CodeGen::Struct *rasterizerData = file.getStruct("RasterizerData");
        rasterizerData->mAnnotations.push_back("semantic");

        file.statement(CodeGen::VariableDefinition { {}, { "OUT", rasterizerData } });

        for (uint32_t i = 0; i < dataInCount(); ++i) {
            rasterizerData->mVariables.push_back({ {}, { std::string { dataInName(i) }, dataInType(i) } });
            file.statement(CodeGen::Assignment { {} , CodeGen::VariableAccess { "OUT." + std::string { dataInName(i) } }, generator.read(i) });
        }

        assert(!generator.mRasterizerData);
        generator.mRasterizerData = rasterizerData;

        std::vector<CodeGen::Function> *vertexInstance = file.nextInstance();

        file.beginFunction("main", { toValueTypeDesc<Vector4>() }, { { "IN", generator.mRasterizerData } });
        NodeGraph::Pin pin = flowOutTarget(0);
        generator.generate(pin);
        file.endFunction();

        file.setInstance(vertexInstance);

        for (CodeGen::Statement &statement : generator.mAdditionalRasterizerStatements) {
            file.statement(std::move(statement));
        }
        generator.mAdditionalRasterizerStatements.clear();

        file.statement(CodeGen::Return { {} ,CodeGen::VariableAccess { "OUT" } });

        flowInOut.reset();
    }

    CodeGen::Statement RasterizerNode::generateRead(NodeGraph::CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<NodeGraph::CodeGeneratorData> &data) const
    {
        return CodeGen::VariableAccess { "IN." + std::string { dataInName(providerIndex) } };
    }

}
}
