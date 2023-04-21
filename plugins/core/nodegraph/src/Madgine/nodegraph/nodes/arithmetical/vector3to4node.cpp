#include "../../../nodegraphlib.h"

#include "vector3to4node.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"
#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "../../codegenerator.h"

NODE(Vector3to4Node, Engine::NodeGraph::Vector3to4Node)

METATABLE_BEGIN_BASE(Engine::NodeGraph::Vector3to4Node, Engine::NodeGraph::NodeBase)
MEMBER(mFillValue)
METATABLE_END(Engine::NodeGraph::Vector3to4Node)

SERIALIZETABLE_INHERIT_BEGIN(Engine::NodeGraph::Vector3to4Node, Engine::NodeGraph::NodeBase)
FIELD(mFillValue)
SERIALIZETABLE_END(Engine::NodeGraph::Vector3to4Node)

namespace Engine {
namespace NodeGraph {

    Vector3to4Node::Vector3to4Node(NodeGraph &graph)
        : Node(graph)
    {
        setup();
    }

    Vector3to4Node::Vector3to4Node(const Vector3to4Node &other, NodeGraph &graph)
        : Node(other, graph)
        , mFillValue(other.mFillValue)
    {
    }

    size_t Vector3to4Node::dataInCount() const
    {
        return 1;
    }

    ExtendedValueTypeDesc Vector3to4Node::dataInType(uint32_t index, bool bidir) const
    {
        return toValueTypeDesc<Vector3>();
    }

    std::string_view Vector3to4Node::dataInName(uint32_t index) const
    {
        return "in";
    }

    size_t Vector3to4Node::dataProviderCount() const
    {
        return 1;
    }

    ExtendedValueTypeDesc Vector3to4Node::dataProviderType(uint32_t index, bool bidir) const
    {
        return toValueTypeDesc<Vector4>();
    }

    std::string_view Vector3to4Node::dataProviderName(uint32_t index) const
    {
        return "out";
    }

    CodeGen::Statement Vector3to4Node::generateRead(CodeGenerator &generator, uint32_t providerIndex, std::unique_ptr<CodeGeneratorData> &data) const
    {
        return CodeGen::Constructor { {}, toValueTypeDesc<Vector4>(), false, { generator.read(0), CodeGen::Constant { ValueType { mFillValue } } } };
    }

}
}