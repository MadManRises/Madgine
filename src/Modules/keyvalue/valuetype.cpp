#include "../moduleslib.h"
#include "valuetype.h"

namespace Engine {

ValueType::ValueType()
{
}

ValueType::ValueType(const ValueType &other)
    : mUnion(other.mUnion)
{
}

ValueType::ValueType(ValueType &&other) noexcept
    : mUnion(std::forward<decltype(other.mUnion)>(other.mUnion))
{
}

ValueType::ValueType(const char *s)
    : ValueType(std::string(s))
{
}

ValueType::~ValueType()
{
    clear();
}

void ValueType::clear()
{
    mUnion = std::monostate {};
}

void ValueType::operator=(const ValueType &other)
{
    mUnion = other.mUnion;
}

void ValueType::operator=(const char *const s)
{
    mUnion = std::string(s);
}

bool ValueType::operator==(const ValueType &other) const
{
    return mUnion == other.mUnion;
}

bool ValueType::operator!=(const ValueType &other) const
{
    return !(*this == other);
}

bool ValueType::operator<(const ValueType &other) const
{
    switch (type()) {
    case Type::StringValue:
        switch (other.type()) {
        case Type::StringValue:
            return std::get<std::string>(mUnion) < std::get<std::string>(other.mUnion);
        default:
            throw ValueTypeException(Database::Exceptions::invalidValueType);
        }
    case Type::IntValue:
        switch (other.type()) {
        case Type::IntValue:
            return std::get<int>(mUnion) < std::get<int>(other.mUnion);
        case Type::FloatValue:
            return std::get<int>(mUnion) < std::get<float>(other.mUnion);
        default:
            throw ValueTypeException(Database::Exceptions::invalidValueType);
        }
    default:
        throw ValueTypeException(Database::Exceptions::invalidValueType);
    }
}

bool ValueType::operator>(const ValueType &other) const
{
    return other < *this;
}

void ValueType::operator+=(const ValueType &other)
{
    switch (type()) {
    case Type::StringValue:
        switch (other.type()) {
        case Type::StringValue:
            std::get<std::string>(mUnion) += std::get<std::string>(other.mUnion);
            return;
        case Type::IntValue:
            std::get<std::string>(mUnion) += std::to_string(std::get<int>(other.mUnion));
        default:
            break;
        }
        break;
    case Type::IntValue:
        switch (other.type()) {
        case Type::IntValue:
            std::get<int>(mUnion) += std::get<int>(other.mUnion);
            return;
        case Type::FloatValue:
            *this = std::get<int>(mUnion) + std::get<float>(other.mUnion);
        default:
            break;
        }
        break;
    default:
        throw ValueTypeException(Database::Exceptions::invalidTypesForOperator("+", getTypeString(), other.getTypeString()));
    }
}

ValueType ValueType::operator+(const ValueType &other) const
{
    ValueType result = *this;
    result += other;
    return result;
}

void ValueType::operator-=(const ValueType &other)
{
    switch (type()) {
    case Type::IntValue:
        switch (other.type()) {
        case Type::IntValue:
            std::get<int>(mUnion) -= std::get<int>(other.mUnion);
            return;
        case Type::FloatValue:
            *this = std::get<int>(mUnion) - std::get<float>(other.mUnion);
        default:
            break;
        }
        break;
    default:
        throw ValueTypeException(Database::Exceptions::invalidTypesForOperator("-", getTypeString(), other.getTypeString()));
    }
}

ValueType ValueType::operator-(const ValueType &other) const
{
    ValueType result = *this;
    result -= other;
    return result;
}

void ValueType::operator/=(const ValueType &other)
{
    switch (type()) {
    case Type::IntValue:
        switch (other.type()) {
        case Type::IntValue:
            std::get<int>(mUnion) /= std::get<int>(other.mUnion);
            return;
        case Type::FloatValue:
            *this = std::get<int>(mUnion) / std::get<float>(other.mUnion);
        default:
            break;
        }
        break;
    case Type::FloatValue:
        switch (other.type()) {
        case Type::IntValue:
            std::get<float>(mUnion) /= std::get<int>(other.mUnion);
            return;
        case Type::FloatValue:
            std::get<float>(mUnion) /= std::get<float>(other.mUnion);
        default:
            break;
        }
        break;
    default:
        throw ValueTypeException(Database::Exceptions::invalidTypesForOperator("/", getTypeString(), other.getTypeString()));
    }
}

ValueType ValueType::operator/(const ValueType &other) const
{
    ValueType result = *this;
    result /= other;
    return result;
}

void ValueType::operator*=(const ValueType &other)
{
    switch (type()) {
    case Type::IntValue:
        switch (other.type()) {
        case Type::IntValue:
            std::get<int>(mUnion) *= std::get<int>(other.mUnion);
            return;
        case Type::FloatValue:
            *this = std::get<int>(mUnion) * std::get<float>(other.mUnion);
        default:
            break;
        }
        break;
    case Type::FloatValue:
        switch (other.type()) {
        case Type::IntValue:
            std::get<float>(mUnion) *= std::get<int>(other.mUnion);
            return;
        case Type::FloatValue:
            std::get<float>(mUnion) *= std::get<float>(other.mUnion);
        default:
            break;
        }
        break;
    default:
        throw ValueTypeException(Database::Exceptions::invalidTypesForOperator("*", getTypeString(), other.getTypeString()));
    }
}

ValueType ValueType::operator*(const ValueType &other) const
{
    ValueType result = *this;
    result *= other;
    return result;
}

std::string ValueType::toString() const
{
    switch (type()) {
    case Type::BoolValue:
        return std::get<bool>(mUnion) ? "true" : "false";
    case Type::StringValue:
        return "\""s + std::get<std::string>(mUnion) + "\"";
    case Type::IntValue:
        return std::to_string(std::get<int>(mUnion));
    case Type::UIntValue:
        return std::to_string(std::get<size_t>(mUnion));
    case Type::NullValue:
        return "NULL";
    case Type::ScopeValue:
        return std::get<TypedScopePtr>(mUnion).name();
    case Type::FloatValue:
        return std::to_string(std::get<float>(mUnion));
    case Type::Vector2Value:
        return "["s + std::to_string(std::get<Vector2>(mUnion).x) + ", " + std::to_string(std::get<Vector2>(mUnion).y);
    case Type::Vector3Value:
        return "["s + std::to_string(std::get<Vector3>(mUnion).x) + ", " + std::to_string(std::get<Vector3>(mUnion).y) + ", " + std::to_string(std::get<Vector3>(mUnion).z) + "]";
    case Type::Vector4Value:
        return "["s + std::to_string(std::get<Vector4>(mUnion)[0]) + ", " + std::to_string(std::get<Vector4>(mUnion)[1]) + ", " + std::to_string(std::get<Vector4>(mUnion)[2]) + ", " + std::to_string(std::get<Vector4>(mUnion)[3]) + "]";
    case Type::QuaternionValue:
        return "{"s + std::to_string(std::get<Quaternion>(mUnion).v.x) + ", " + std::to_string(std::get<Quaternion>(mUnion).v.y) + ", " + std::to_string(std::get<Quaternion>(mUnion).v.z) + ", " + std::to_string(std::get<Quaternion>(mUnion).w) + "}";
    case Type::ApiMethodValue:
        return "<method>";
    case Type::BoundApiMethodValue:
        return "<boundmethod>";
    default:
        throw ValueTypeException("Unknown Type!");
    }
}

std::string ValueType::getTypeString() const
{
    return getTypeString(type());
}

std::string ValueType::getTypeString(Type type)
{
    switch (type) {
    case Type::BoolValue:
        return "Bool";
    case Type::FloatValue:
        return "Float";
    case Type::IntValue:
        return "Integer";
    case Type::UIntValue:
        return "Unsigned Integer";
    case Type::NullValue:
        return "Null-Type";
    case Type::ScopeValue:
        return "Scope";
    case Type::StringValue:
        return "String";
    case Type::Matrix3Value:
        return "Matrix3x3";
    case Type::Vector2Value:
        return "Vector2";
    case Type::Vector3Value:
        return "Vector3";
    case Type::Vector4Value:
        return "Vector4";
    case Type::QuaternionValue:
        return "Quaternion";
    case Type::ApiMethodValue:
        return "Method";
    case Type::KeyValueVirtualIteratorValue:
        return "Iterator";
    case Type::BoundApiMethodValue:
        return "Bound Method";
    case Type::ObjectValue:
        return "Object";
    default:
        std::terminate();
    }
}

ValueType::Type ValueType::type() const
{
    return static_cast<Type>(mUnion.index());
}
}

std::ostream &operator<<(std::ostream &stream,
    const Engine::ValueType &v)
{
    return stream << v.toString();
}
