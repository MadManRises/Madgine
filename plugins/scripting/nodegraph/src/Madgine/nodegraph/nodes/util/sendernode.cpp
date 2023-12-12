#include "../../../nodegraphlib.h"

#include "sendernode.h"

#include "Generic/execution/algorithm.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/keyvalueutil/valuetypeserialize.h"
#include "Meta/serialize/serializetable_impl.h"

using ForEachNode = Engine::NodeGraph::SenderNode<Engine::Execution::for_each_t>;

NAMED_UNIQUECOMPONENT(ForEach, ForEachNode)

REGISTER_TYPE(ForEachNode)

METATABLE_BEGIN_BASE(ForEachNode, Engine::NodeGraph::NodeBase)
PROPERTY(Arguments, getArguments<0>, setArguments<0>)
METATABLE_END(ForEachNode)

SERIALIZETABLE_INHERIT_BEGIN(ForEachNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(ForEachNode)

using LetValueNode = Engine::NodeGraph::SenderNode<Engine::Execution::let_value_t>;

NAMED_UNIQUECOMPONENT(LetValue, LetValueNode)

REGISTER_TYPE(LetValueNode)

METATABLE_BEGIN_BASE(LetValueNode, Engine::NodeGraph::NodeBase)
METATABLE_END(LetValueNode)

SERIALIZETABLE_INHERIT_BEGIN(LetValueNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(LetValueNode)

using AddNode = Engine::NodeGraph::SenderNode<Engine::Execution::then_t::typed<Engine::NodeGraph::Add>>;

NAMED_UNIQUECOMPONENT(Add, AddNode)

REGISTER_TYPE(AddNode)

METATABLE_BEGIN_BASE(AddNode, Engine::NodeGraph::NodeBase)
METATABLE_END(AddNode)

SERIALIZETABLE_INHERIT_BEGIN(AddNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(AddNode)

using DivideNode = Engine::NodeGraph::SenderNode<Engine::Execution::then_t::typed<Engine::NodeGraph::Divide>>;

NAMED_UNIQUECOMPONENT(Divide, DivideNode)

REGISTER_TYPE(DivideNode)

METATABLE_BEGIN_BASE(DivideNode, Engine::NodeGraph::NodeBase)
METATABLE_END(DivideNode)

SERIALIZETABLE_INHERIT_BEGIN(DivideNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(DivideNode)

using LogNode = Engine::NodeGraph::SenderNode<Engine::Execution::then_t::typed<Engine::NodeGraph::Log>>;

NAMED_UNIQUECOMPONENT(Log, LogNode)

REGISTER_TYPE(LogNode)

METATABLE_BEGIN_BASE(LogNode, Engine::NodeGraph::NodeBase)
METATABLE_END(LogNode)

SERIALIZETABLE_INHERIT_BEGIN(LogNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(LogNode)

using JustNode = Engine::NodeGraph::SenderNode<Engine::Execution::just_t>;

NAMED_UNIQUECOMPONENT(Just, JustNode)

REGISTER_TYPE(JustNode)

METATABLE_BEGIN_BASE(JustNode, Engine::NodeGraph::NodeBase)
PROPERTY(Value, getArguments<0>, setArguments<0>)
METATABLE_END(JustNode)

SERIALIZETABLE_INHERIT_BEGIN(JustNode, Engine::NodeGraph::NodeBase)
ENCAPSULATED_FIELD(Value, getArguments<0>, setArguments<0>)
SERIALIZETABLE_END(JustNode)

using SequenceNode = Engine::NodeGraph::SenderNode<Engine::Execution::sequence_t>;

NAMED_UNIQUECOMPONENT(Sequence, SequenceNode)

REGISTER_TYPE(SequenceNode)

METATABLE_BEGIN_BASE(SequenceNode, Engine::NodeGraph::NodeBase)
METATABLE_END(SequenceNode)

SERIALIZETABLE_INHERIT_BEGIN(SequenceNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(SequenceNode)

using VariableNode = Engine::NodeGraph::SenderNode<Engine::Execution::Variable_t>;

NAMED_UNIQUECOMPONENT(Variable, VariableNode)

REGISTER_TYPE(VariableNode)

METATABLE_BEGIN_BASE(VariableNode, Engine::NodeGraph::NodeBase)
PROPERTY(Name, getDynamicName<"Name">, setDynamicName<"Name">)
PROPERTY(DefaultValue, getArguments<1>, setArguments<1>)
METATABLE_END(VariableNode)

SERIALIZETABLE_INHERIT_BEGIN(VariableNode, Engine::NodeGraph::NodeBase)
ENCAPSULATED_FIELD(Name, getDynamicName<"Name">, setDynamicName<"Name">)
ENCAPSULATED_FIELD(DefaultValue, getArguments<1>, setArguments<1>)
SERIALIZETABLE_END(VariableNode)

using WriteVarNode = Engine::NodeGraph::SenderNode<Engine::Execution::write_var_t>;

NAMED_UNIQUECOMPONENT(WriteVar, WriteVarNode)

REGISTER_TYPE(WriteVarNode)

METATABLE_BEGIN_BASE(WriteVarNode, Engine::NodeGraph::NodeBase)
PROPERTY(Name, getDynamicName<"Name">, setDynamicName<"Name">)
METATABLE_END(WriteVarNode)

SERIALIZETABLE_INHERIT_BEGIN(WriteVarNode, Engine::NodeGraph::NodeBase)
ENCAPSULATED_FIELD(Name, getDynamicName<"Name">, setDynamicName<"Name">)
SERIALIZETABLE_END(WriteVarNode)

using ReadVarNode = Engine::NodeGraph::SenderNode<Engine::Execution::read_var_t>;

NAMED_UNIQUECOMPONENT(ReadVar, ReadVarNode)

REGISTER_TYPE(ReadVarNode)

METATABLE_BEGIN_BASE(ReadVarNode, Engine::NodeGraph::NodeBase)
PROPERTY(Name, getDynamicName<"Name">, setDynamicName<"Name">)
METATABLE_END(ReadVarNode)

SERIALIZETABLE_INHERIT_BEGIN(ReadVarNode, Engine::NodeGraph::NodeBase)
ENCAPSULATED_FIELD(Name, getDynamicName<"Name">, setDynamicName<"Name">)
SERIALIZETABLE_END(ReadVarNode)

using AssignNode = Engine::NodeGraph::SenderNode<Engine::Execution::assign_t>;

NAMED_UNIQUECOMPONENT(Assign, AssignNode)

REGISTER_TYPE(AssignNode)

METATABLE_BEGIN_BASE(AssignNode, Engine::NodeGraph::NodeBase)
METATABLE_END(AssignNode)

SERIALIZETABLE_INHERIT_BEGIN(AssignNode, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(AssignNode)

using Vector3To4Node = Engine::NodeGraph::SenderNode<Engine::Execution::then_t::typed<Engine::NodeGraph::Vector3To4>>;

NAMED_UNIQUECOMPONENT(Vector3To4, Vector3To4Node)

REGISTER_TYPE(Vector3To4Node)

METATABLE_BEGIN_BASE(Vector3To4Node, Engine::NodeGraph::NodeBase)
METATABLE_END(Vector3To4Node)

SERIALIZETABLE_INHERIT_BEGIN(Vector3To4Node, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(Vector3To4Node)

using BreakVector3Node = Engine::NodeGraph::SenderNode<Engine::Execution::then_t::typed<Engine::NodeGraph::BreakVector3>>;

NAMED_UNIQUECOMPONENT(BreakVector3, BreakVector3Node)

REGISTER_TYPE(BreakVector3Node)

METATABLE_BEGIN_BASE(BreakVector3Node, Engine::NodeGraph::NodeBase)
METATABLE_END(BreakVector3Node)

SERIALIZETABLE_INHERIT_BEGIN(BreakVector3Node, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(BreakVector3Node)

using BreakVector4Node = Engine::NodeGraph::SenderNode<Engine::Execution::then_t::typed<Engine::NodeGraph::BreakVector4>>;

NAMED_UNIQUECOMPONENT(BreakVector4, BreakVector4Node)

REGISTER_TYPE(BreakVector4Node)

METATABLE_BEGIN_BASE(BreakVector4Node, Engine::NodeGraph::NodeBase)
METATABLE_END(BreakVector4Node)

SERIALIZETABLE_INHERIT_BEGIN(BreakVector4Node, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(BreakVector4Node)

using MakeVector3Node = Engine::NodeGraph::SenderNode<Engine::Execution::then_t::typed<Engine::NodeGraph::MakeVector3>>;

NAMED_UNIQUECOMPONENT(MakeVector3, MakeVector3Node)

REGISTER_TYPE(MakeVector3Node)

METATABLE_BEGIN_BASE(MakeVector3Node, Engine::NodeGraph::NodeBase)
METATABLE_END(MakeVector3Node)

SERIALIZETABLE_INHERIT_BEGIN(MakeVector3Node, Engine::NodeGraph::NodeBase)
SERIALIZETABLE_END(MakeVector3Node)

