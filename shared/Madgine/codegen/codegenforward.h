#pragma once

namespace CodeGen {

struct Assignment;
struct Return;
struct VariableRead;
struct VariableDefinition;
struct MemberAccess;
struct CustomCodeBlock;
struct Namespace;
struct Function;
struct ArithOperation;
struct Constructor;
struct Constant;
struct Comment;

struct Struct;

using Statement = std::variant<Assignment, Return, VariableRead, CustomCodeBlock, Namespace, MemberAccess, VariableDefinition, ArithOperation, Constructor, Constant, Comment>;
using Type = std::variant<Engine::ValueTypeDesc, Struct *>;

struct ShaderFile;
struct CppFile;
struct File;

}