#include "../../../nodegraphlib.h"


#include "Generic/execution/algorithm.h"

#include "Meta/keyvalueutil/valuetypeserialize.h"

#include "sendernode_impl.h"

DEFAULT_SENDER_NODE_BEGIN(ForEach, Engine::Execution::for_each, std::vector<int>, Engine::NodeGraph::NodeAlgorithm<1, Engine::ValueType>)
ARGUMENT(Arguments, 0)
SENDER_NODE_END(ForEach)

DEFAULT_SENDER_NODE_BEGIN(LetValue, Engine::Execution::let_value, Engine::NodeGraph::NodeReader<Engine::ValueType>, Engine::NodeGraph::NodeAlgorithm<0, Engine::ValueType>)
SENDER_NODE_END(LetValue)

CONSTANT_SENDER_NODE_BEGIN(Add, Engine::Execution::reduce_stream, Engine::NodeGraph::NodeStream<int>, std::integral_constant<int, 0>, Engine::NodeGraph::Add)
SENDER_NODE_END(Add)

CONSTANT_SENDER_NODE_BEGIN(Divide, Engine::Execution::reduce_stream, Engine::NodeGraph::NodeStream<int>, std::integral_constant<int, 1>, Engine::NodeGraph::Divide)
SENDER_NODE_END(Divide)

DEFAULT_SENDER_NODE_BEGIN(Log, Engine::Execution::then, Engine::NodeGraph::NodeReader<Engine::ValueType>, Engine::NodeGraph::Log)
SENDER_NODE_END(Log)

CONSTANT_SENDER_NODE_BEGIN(Just, Engine::Execution::just, Engine::ValueType);
ARGUMENT(Value, 0)
SENDER_NODE_END(Just)

VARIABLE_SENDER_NODE_BEGIN(Variable, Engine::Execution::Variable<"Name">, Engine::NodeGraph::NodeReader<>, Engine::ValueType)
DYNAMIC_NAME(Name)
ARGUMENT(DefaultValue, 0)
SENDER_NODE_END(Variable)

DEFAULT_VARIABLE_ACCESS_SENDER_NODE_BEGIN(WriteVar, Engine::Execution::write_var<"Name">, Engine::NodeGraph::NodeReader<Engine::ValueType>)
DYNAMIC_NAME(Name)
SENDER_NODE_END(WriteVar)

CONSTANT_VARIABLE_ACCESS_SENDER_NODE_BEGIN(ReadVar, Engine::Execution::read_var<"Name", Engine::ValueType>)
DYNAMIC_NAME(Name)
SENDER_NODE_END(ReadVar)

CONSTANT_SENDER_NODE_BEGIN(Vector3To4, Engine::Execution::then, Engine::NodeGraph::NodeReader<Engine::Vector3, float>, Engine::NodeGraph::Vector3To4)
SENDER_NODE_END(Vector3To4)

CONSTANT_SENDER_NODE_BEGIN(BreakVector3, Engine::Execution::then, Engine::NodeGraph::NodeReader<Engine::Vector3>, Engine::NodeGraph::BreakVector3)
SENDER_NODE_END(BreakVector3)

CONSTANT_SENDER_NODE_BEGIN(BreakVector4, Engine::Execution::then, Engine::NodeGraph::NodeReader<Engine::Vector4>, Engine::NodeGraph::BreakVector4)
SENDER_NODE_END(BreakVector4)

CONSTANT_SENDER_NODE_BEGIN(MakeVector3, Engine::Execution::then, Engine::NodeGraph::NodeReader<float, float, float>, Engine::NodeGraph::MakeVector3)
SENDER_NODE_END(MakeVector3)

/*using SequenceNode = Engine::NodeGraph::SenderNode<Engine::Execution::sequence, false, Engine::type_pack<>, Engine::type_pack<>, Engine::Execution::recursive<Engine::NodeGraph::NodeSender<1>>>;

NAMED_UNIQUECOMPONENT(Sequence, SequenceNode)

REGISTER_TYPE(SequenceNode)

METATABLE_BEGIN_BASE(SequenceNode, Engine::NodeGraph::NodeBase)
METATABLE_END(SequenceNode)

SERIALIZETABLE_INHERIT_BEGIN(SequenceNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(SequenceNode)*/
