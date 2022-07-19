#include "codegenlib.h"

#include "statement.h"

namespace CodeGen {

Return::Return(Statement statement)
    : mStatement(std::make_unique<Statement>(std::move(statement)))
{
}

Return::Return(const Return &other)
    : mStatement(std::make_unique<Statement>(*other.mStatement))
{
}

Return &Return::operator=(const Return &other)
{
    *mStatement = *other.mStatement;
    return *this;
}

Function::Function(std::string_view name, Type returnType, std::vector<Variable> arguments, std::vector<std::string> annotations)
    : mName(name)
    , mReturnType(returnType)
    , mArguments(std::move(arguments))
    , mAnnotations(std::move(annotations))
{
}

MemberAccess::MemberAccess(std::string_view memberName, Statement statement)
    : mMemberName(memberName)
    , mStatement(std::make_unique<Statement>(std::move(statement)))
{
}

MemberAccess::MemberAccess(const MemberAccess &other)
    : mMemberName(other.mMemberName)
    , mStatement(std::make_unique<Statement>(*other.mStatement))
{
}

MemberAccess &MemberAccess::operator=(const MemberAccess &other)
{
    mMemberName = other.mMemberName;
    *mStatement = *other.mStatement;
    return *this;
}

Struct::Struct(std::string_view name)
    : mName(name)
{
}

Assignment::Assignment(std::string_view variableName, Statement statement)
    : mVariableName(variableName)
    , mStatement(std::make_unique<Statement>(std::move(statement)))
{
}

Assignment::Assignment(const Assignment &other)
    : mVariableName(other.mVariableName)
    , mStatement(std::make_unique<Statement>(*other.mStatement))
{
}

Assignment &Assignment::operator=(const Assignment &other)
{
    mVariableName = other.mVariableName;
    *mStatement = *other.mStatement;
    return *this;
}

StatementVector::StatementVector() = default;
StatementVector::StatementVector(std::initializer_list<Statement> statements)
    : mVector(std::move(statements))
{
}
StatementVector::StatementVector(const StatementVector &) = default;
StatementVector::~StatementVector() = default;
StatementVector &StatementVector::operator=(const StatementVector &) = default;

std::vector<Statement>::const_iterator StatementVector::begin() const
{
    return mVector.begin();
}

std::vector<Statement>::const_iterator StatementVector::end() const
{
    return mVector.end();
}

}