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

ValueType::ValueType(std::string &&s)
    : mUnion(std::in_place_type<HeapObject<std::string>>, std::move(s))
{
}

ValueType::ValueType(const std::string &s)
    : mUnion(std::in_place_type<std::string_view>, s)
{
}

ValueType::ValueType(const std::string &&s)
    : mUnion(std::in_place_type<HeapObject<std::string>>, s)
{
}

ValueType::ValueType(Matrix3 &&m)
    : mUnion(CoW<Matrix3> { std::move(m) })
{
}

ValueType::ValueType(const Matrix3 &m)
    : mUnion(CoW<Matrix3> { m })
{
}

ValueType::ValueType(const Matrix3 &&m)
    : mUnion(CoW<Matrix3> { std::move(m) })
{
}

ValueType::ValueType(Matrix4 &&m)
    : mUnion(CoW<Matrix4> { std::move(m) })
{
}

ValueType::ValueType(const Matrix4 &m)
    : mUnion(CoW<Matrix4> { m })
{
}

ValueType::ValueType(const Matrix4 &&m)
    : mUnion(CoW<Matrix4> { std::move(m) })
{
}

/*ValueType::ValueType(const char *s)
    : ValueType(std::string(s))
{
}*/

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

void ValueType::operator=(ValueType &&other)
{
    mUnion = std::move(other.mUnion);
}

void ValueType::operator=(const std::string &s)
{
    mUnion = Union { std::in_place_type<std::string_view>,
        s };
}

void ValueType::operator=(std::string &s)
{
    mUnion = Union { std::in_place_type<std::string_view>,
        s };
}

void ValueType::operator=(const std::string &&s)
{
    mUnion = Union { std::in_place_type<HeapObject<std::string>>,
        s };
}

void ValueType::operator=(std::string &&s)
{
    mUnion = Union { std::in_place_type<HeapObject<std::string>>,
        std::move(s) };
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
    case Type::OwningStringValue:
        switch (other.type()) {
        case Type::OwningStringValue:
            return *std::get<HeapObject<std::string>>(mUnion) < *std::get<HeapObject<std::string>>(other.mUnion);
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
    case Type::OwningStringValue:
        switch (other.type()) {
        case Type::OwningStringValue:
            *std::get<HeapObject<std::string>>(mUnion) += *std::get<HeapObject<std::string>>(other.mUnion);
            return;
        case Type::IntValue:
            *std::get<HeapObject<std::string>>(mUnion) += std::to_string(std::get<int>(other.mUnion));
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
    case Type::OwningStringValue:
        return "\""s + *std::get<HeapObject<std::string>>(mUnion) + "\"";
    case Type::NonOwningStringValue:
        return "\""s + std::string { std::get<std::string_view>(mUnion) } + "\"";
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
    case Type::Matrix3Value:
        return "[ ["s + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[0][0]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[0][1]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[0][2]) + "], [" + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[1][0]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[1][1]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[1][2]) + "], [" + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[2][0]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[2][1]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[2][2]) + "] ]";
    case Type::Matrix4Value:
        return "[ ["s + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[0][0]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[0][1]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[0][2]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[0][3]) + "], [" + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[1][0]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[1][1]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[1][2]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[1][3]) + "], [" + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[2][0]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[2][1]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[2][2]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[2][3]) + "], [" + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[3][0]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[3][1]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[3][2]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[3][3]) + "] ]";
    case Type::ApiMethodValue:
        return "<method>";
    default:
        throw ValueTypeException("Unknown Type!");
    }
}

std::string ValueType::toShortString() const
{
    switch (type()) {
    case Type::BoolValue:
        return std::get<bool>(mUnion) ? "true" : "false";
    case Type::OwningStringValue:
        return "\""s + *std::get<HeapObject<std::string>>(mUnion) + "\"";
    case Type::NonOwningStringValue:
        return "\""s + std::string { std::get<std::string_view>(mUnion) } + "\"";
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
    case Type::Matrix3Value:
        return "Matrix3[...]";
    case Type::Matrix4Value:
        return "Matrix4[...]";
    case Type::ApiMethodValue:
        return "<method>";
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
#define VALUETYPE_SEP
#define VALUETYPE_TYPE(T, Storage, Name) \
    case Type::Name##Value:              \
        return #Name;
#include "valuetypedef.h"
    default:
        std::terminate();
    }
}

ValueType::Type ValueType::type() const
{
    return static_cast<Type>(mUnion.index());
}

ValueTypeRef::ValueTypeRef(ValueTypeRef &&other)
    : mValue(std::move(other.mValue))
    , mData(std::exchange(other.mData, nullptr))
{
}

const ValueType &ValueTypeRef::value() const
{
    return mValue;
}

ValueTypeRef::operator const ValueType &() const
{
    return mValue;
}

bool ValueTypeRef::isEditable() const
{
    return mData != nullptr;
}

ValueTypeRef &ValueTypeRef::operator=(const ValueType &v)
{
    if (v.type() != mValue.type() || v.is<TypedScopePtr>())
        std::terminate();

    mValue = v;
    mValue.visit(overloaded {
#define VALUETYPE_SEP ,
#define VALUETYPE_TYPE(T, Storage, Name) \
    [this](const T &t) {                 \
        *static_cast<T *>(mData) = t;    \
    }
#include "valuetypedef.h"
    });

    return *this;
}

ValueTypeRef& ValueTypeRef::operator=(ValueTypeRef&& other) {
    mValue = std::move(other.mValue);
    mData = std::exchange(other.mData, nullptr);
    return *this;
}

}

std::ostream &operator<<(std::ostream &stream,
    const Engine::ValueType &v)
{
    return stream << v.toString();
}
