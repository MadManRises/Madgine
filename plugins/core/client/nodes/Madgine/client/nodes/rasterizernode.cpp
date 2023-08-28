#include "../../clientnodeslib.h"

#include "rasterizernode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "shadercodegenerator.h"

#include "Madgine/nodegraph/nodegraph.h"

#include "Madgine/nodegraph/nodes/util/sendernode.h"

NODE(RasterizerNode, Engine::Render::RasterizerNode)

METATABLE_BEGIN_BASE(Engine::Render::RasterizerNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::Render::RasterizerNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Render::RasterizerNode, Engine::NodeGraph::NodeBase)
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
    {
    }

    size_t RasterizerNode::flowInCount(uint32_t group) const
    {
        return 1;
    }

    std::string_view RasterizerNode::flowInName(uint32_t index, uint32_t group) const
    {
        return "vertex";
    }

    uint32_t RasterizerNode::flowInMask(uint32_t index, uint32_t group, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    size_t RasterizerNode::flowOutBaseCount(uint32_t group) const
    {
        return 1;
    }

    std::string_view RasterizerNode::flowOutName(uint32_t index, uint32_t group) const
    {
        return "pixel";
    }

    uint32_t RasterizerNode::flowOutMask(uint32_t index, uint32_t group, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    size_t RasterizerNode::dataProviderBaseCount(uint32_t group) const
    {
        return dataInBaseCount();
    }

    std::string_view RasterizerNode::dataProviderName(uint32_t index, uint32_t group) const
    {
        return dataInName(index);
    }

    ExtendedValueTypeDesc RasterizerNode::dataProviderType(uint32_t index, uint32_t group, bool bidir) const
    {
        if (index == 0)
            return toValueTypeDesc<Vector4>();
        if (index == dataProviderCount(group))
            return { ExtendedValueTypeEnum::GenericType };
        ExtendedValueTypeDesc desc = { ExtendedValueTypeEnum::GenericType };
        if (index <= dataProviderCount(group)) {
            NodeGraph::Pin pin = dataInSource(index);
            desc = mGraph.dataProviderType(pin, false);
        }
        if (bidir && desc.mType == ExtendedValueTypeEnum::GenericType)
            return dataInType(index, false);
        return desc;
    }

    uint32_t RasterizerNode::dataProviderMask(uint32_t index, uint32_t group, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    bool RasterizerNode::dataProviderVariadic(uint32_t group) const
    {
        return true;
    }

    size_t RasterizerNode::dataInBaseCount(uint32_t group) const
    {
        return 1;
    }

    std::string_view RasterizerNode::dataInName(uint32_t index, uint32_t group) const
    {
        if (index == 0)
            return "pos";
        if (index < dataInCount(group)) {
            NodeGraph::Pin pin = dataInSource(index, group);
            return mGraph.dataProviderName(pin);
        }
        if (index == dataInCount(group))
            return "<>";
        throw 0;
    }

    ExtendedValueTypeDesc RasterizerNode::dataInType(uint32_t index, uint32_t group, bool bidir) const
    {
        if (index == 0)
            return toValueTypeDesc<Vector4>();
        if (index == dataInCount(group))
            return { ExtendedValueTypeEnum::GenericType };
        ExtendedValueTypeDesc desc = { ExtendedValueTypeEnum::GenericType };
        if (index < dataInCount(group)) {
            if (!mDataProviderPins[0][index].mTargets.empty()) {
                NodeGraph::Pin pin = mDataProviderPins[0][index].mTargets.front();
                desc = mGraph.dataInType(pin, false);
            }
        }
        if (bidir && desc.mType == ExtendedValueTypeEnum::GenericType)
            return dataProviderType(index, false);
        return desc;
    }

    uint32_t RasterizerNode::dataInMask(uint32_t index, uint32_t group, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    bool RasterizerNode::dataInVariadic(uint32_t group) const
    {
        return true;
    }

    void RasterizerNode::interpret(NodeGraph::NodeReceiver<NodeGraph::NodeBase> receiver, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const
    {
        throw 0;
    }

    void RasterizerNode::generate(NodeGraph::CodeGenerator &_generator, std::unique_ptr<NodeGraph::CodeGeneratorData> &data, uint32_t flowIn, uint32_t group) const
    {
        assert(flowIn == 0);
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
            file.statement(CodeGen::Assignment { {} , CodeGen::VariableAccess { "OUT." + std::string { dataInName(i) } }, generator.read(dataInSource(i)) });
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


    }

    CodeGen::Statement RasterizerNode::generateRead(NodeGraph::CodeGenerator &generator, std::unique_ptr<NodeGraph::CodeGeneratorData> &data, uint32_t providerIndex, uint32_t group) const
    {
        return CodeGen::VariableAccess { "IN." + std::string { dataInName(providerIndex) } };
    }

}
}
