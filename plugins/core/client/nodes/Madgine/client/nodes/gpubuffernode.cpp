#include "../../clientnodeslib.h"

#include "gpubuffernode.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "shadercodegenerator.h"

#include "Meta/math/vector4.h"

#include "Madgine/nodegraph/nodegraph.h"
#include "Madgine/nodegraph/nodes/util/sendernode.h"

NODE(GPUBufferNode, Engine::Render::GPUBufferNode)

METATABLE_BEGIN_BASE(Engine::Render::GPUBufferNode, Engine::NodeGraph::NodeBase)
METATABLE_END(Engine::Render::GPUBufferNode)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Render::GPUBufferNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Engine::Render::GPUBufferNode)

namespace Engine {
namespace Render {

    GPUBufferNode::GPUBufferNode(NodeGraph::NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    GPUBufferNode::GPUBufferNode(const GPUBufferNode &other, NodeGraph::NodeGraph &graph)
        : Node(other, graph)
    {
    }

    size_t GPUBufferNode::calculateSize() const
    {
        return sizeof(Matrix4) + sizeof(Vector4);
    }

    size_t GPUBufferNode::flowInCount(uint32_t group) const
    {
        return 1;
    }

    std::string_view GPUBufferNode::flowInName(uint32_t index, uint32_t group) const
    {
        return "write";
    }

    size_t GPUBufferNode::flowOutBaseCount(uint32_t group) const
    {
        return 1;
    }

    std::string_view GPUBufferNode::flowOutName(uint32_t index, uint32_t group) const
    {
        return "return";
    }

    size_t GPUBufferNode::dataProviderBaseCount(uint32_t group) const
    {
        return 0;
    }

    std::string_view GPUBufferNode::dataProviderName(uint32_t index, uint32_t group) const
    {
        return dataInName(index, group);
    }

    ExtendedValueTypeDesc GPUBufferNode::dataProviderType(uint32_t index, uint32_t group, bool bidir) const
    {
        if (index == dataProviderCount(group))
            return { ExtendedValueTypeEnum::GenericType };
        ExtendedValueTypeDesc desc { ExtendedValueTypeEnum::GenericType };
        NodeGraph::Pin pin = mDataInPins[0][index].mSource;
        desc = mGraph.dataProviderType(pin, false);
        if (bidir && desc.mType == ExtendedValueTypeEnum::GenericType)
            return dataInType(index, group, false);
        return desc;
    }

    uint32_t GPUBufferNode::dataProviderMask(uint32_t index, uint32_t group, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::GPU;
    }

    size_t GPUBufferNode::dataInBaseCount(uint32_t group) const
    {
        return 0;
    }

    std::string_view GPUBufferNode::dataInName(uint32_t index, uint32_t group) const
    {
        if (index < dataProviderCount(group)) {
            NodeGraph::Pin pin = mDataInPins[0][index].mSource;
            return pin ? mGraph.dataProviderName(pin) : "??";
        }
        if (index == dataProviderCount(group))
            return "<>";
        throw 0;
    }

    ExtendedValueTypeDesc GPUBufferNode::dataInType(uint32_t index, uint32_t group, bool bidir) const
    {
        if (index == dataProviderCount(group))
            return { ExtendedValueTypeEnum::GenericType };
        ExtendedValueTypeDesc desc { ExtendedValueTypeEnum::GenericType };
        if (!mDataProviderPins[0][index].mTargets.empty()) {
            NodeGraph::Pin pin = mDataProviderPins[0][index].mTargets.front();
            desc = mGraph.dataInType(pin, false);
        }
        if (bidir && desc.mType == ExtendedValueTypeEnum::GenericType)
            return dataProviderType(index, false);
        return desc;
    }

    uint32_t GPUBufferNode::dataInMask(uint32_t index, uint32_t group, bool bidir) const
    {
        return NodeGraph::NodeExecutionMask::CPU;
    }

    bool GPUBufferNode::dataInVariadic(uint32_t group) const
    {
        return true;
    }

    void GPUBufferNode::interpret(NodeGraph::NodeReceiver receiver, std::unique_ptr<NodeGraph::NodeInterpreterData> &data, uint32_t flowIn, uint32_t group) const
    {
        assert(flowIn == 0);

        if (!data) {
            data = std::make_unique<GPUBufferInterpreterData>();
        }

        GPUBufferInterpreterData *bufferData = static_cast<GPUBufferInterpreterData *>(data.get());

        if (!bufferData->mMapper) {
            size_t size = calculateSize();
            bufferData->mBuffer = { std::make_unique<char[]>(size), size };
        } else {
            bufferData->mBuffer = bufferData->mMapper();
        }

        void *ptr = bufferData->mBuffer.mData;

        if (ptr) {
            //for (uint32_t i = 0; i < mAdditionalPins; ++i) {
                /* ValueType v;
                receiver.read(v, i);

                v.visit([&]<typename T>(const T &v) {
                    *static_cast<MakeOwning_t<T> *>(ptr) = v;
                    ptr = reinterpret_cast<char *>(ptr) + sizeof(MakeOwning_t<T>);
                });*/
                throw 0;
            //}
        }

        if (bufferData->mMapper)
            bufferData->mBuffer.clear();

        receiver.set_value();
    }

    void GPUBufferNode::generate(NodeGraph::CodeGenerator &_generator, IndexType<uint32_t> &flowInOut, std::unique_ptr<NodeGraph::CodeGeneratorData> &data) const
    {
        throw 0;
    }

    CodeGen::Statement GPUBufferNode::generateRead(NodeGraph::CodeGenerator &_generator, uint32_t providerIndex, std::unique_ptr<NodeGraph::CodeGeneratorData> &data) const
    {

        if (!data) {
            ShaderCodeGenerator &generator = dynamic_cast<ShaderCodeGenerator &>(_generator);

            std::unique_ptr<GPUBufferCodeGeneratorData> _data = std::make_unique<GPUBufferCodeGeneratorData>();
            _data->mInterpretData = static_cast<GPUBufferInterpreterData *>(generator.mInterpreter->data(mGraph.nodeIndex(this)).get());
            assert(_data->mInterpretData);

            _data->mInstanceIndex = 2;

            generator.registerBuffer(*_data);

            CodeGen::ShaderFile &file = generator.mFile;

            CodeGen::Struct *bufferStruct = file.getStruct("buffer" + std::to_string(_data->mIndex));
            bufferStruct->mAnnotations.push_back("buffer" + std::to_string(_data->mIndex));

            for (uint32_t i = 0; i < dataInCount(); ++i) {
                bufferStruct->mVariables.push_back({ {}, { std::string { dataInName(i) }, dataInType(i) } });
            }

            data = std::move(_data);
        }

        return CodeGen::VariableAccess { std::string { dataProviderName(providerIndex) } };
    }

}
}
