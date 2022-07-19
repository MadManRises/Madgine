#pragma once

#include "Generic/bits.h"
#include "Meta/keyvalue/valuetype.h"

namespace CodeGen {

struct MADGINE_CODEGEN_EXPORT StatementVector {
    StatementVector();
    StatementVector(std::initializer_list<Statement> statements);
    StatementVector(const StatementVector &);
    ~StatementVector();

    StatementVector &operator=(const StatementVector &);

    std::vector<Statement>::const_iterator begin() const;
    std::vector<Statement>::const_iterator end() const;

private:
    std::vector<Statement> mVector;
};

struct Variable {
    std::string mName;
    Type mType;
    Engine::BitArray<64> mConditionals;
    std::vector<std::string> mAnnotations;
};

struct CustomCodeBlock {
    std::string mCode;
    Engine::BitArray<64> mConditionals;
};

struct Namespace {
    std::string mName;
    Engine::BitArray<64> mConditionals;
    std::list<Statement> mElements;
};

struct MADGINE_CODEGEN_EXPORT Assignment {
    Assignment(std::string_view variableName, Statement statement);
    Assignment(const Assignment &other);

    Assignment &operator=(const Assignment &other);

    std::string mVariableName;
    std::unique_ptr<Statement> mStatement;
};

struct MADGINE_CODEGEN_EXPORT Return {
    Return(Statement statement);
    Return(const Return &other);

    Return &operator=(const Return &other);

    std::unique_ptr<Statement> mStatement;
};

struct MADGINE_CODEGEN_EXPORT VariableRead {
    std::string mVariableName;
};

struct MADGINE_CODEGEN_EXPORT VariableDefinition {
    Variable mVariable;
    Engine::ValueType mDefaultValue;
};

struct MADGINE_CODEGEN_EXPORT MemberAccess {
    MemberAccess(std::string_view memberName, Statement statement);
    MemberAccess(const MemberAccess &other);

    MemberAccess &operator=(const MemberAccess &other);

    std::string mMemberName;
    std::unique_ptr<Statement> mStatement;
};

struct MADGINE_CODEGEN_EXPORT Constructor {
    Type mType;
    StatementVector mArguments;
};

struct MADGINE_CODEGEN_EXPORT Constant {
    Engine::ValueType mValue;
};

struct MADGINE_CODEGEN_EXPORT ArithOperation {

    enum {
        ADD,
        MUL
    } mType;
    StatementVector mOperands;
};

struct MADGINE_CODEGEN_EXPORT Function {
    Function(std::string_view name, Type returnType, std::vector<Variable> arguments, std::vector<std::string> annotations);
    Function(const Function &) = delete;
    Function(Function &&) = default;

    std::string mName;
    Type mReturnType;
    std::vector<Variable> mArguments;
    std::list<Statement> mStatements;
    std::vector<std::string> mAnnotations;
};

struct Struct {
    Struct(std::string_view name);

    std::string mName;
    std::vector<Variable> mVariables;
    std::vector<std::string> mAnnotations;
};

}