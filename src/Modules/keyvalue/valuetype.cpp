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
    case Type::OwningStringValue:
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
    if (v.type() != mValue.type())
        std::terminate();

    mValue = v;
    mValue.visit(overloaded {
        [](const std::monostate &) {

        },
        [this](const std::string &s) {
            *static_cast<std::string *>(mData) = s;
        },
        [this](const std::string_view &s) {
            *static_cast<std::string_view *>(mData) = s;
        },
        [this](bool b) {
            *static_cast<bool *>(mData) = b;
        },
        [this](int i) {
            *static_cast<int *>(mData) = i;
        },
        [this](size_t s) {
            *static_cast<size_t *>(mData) = s;
        },
        [this](float f) {
            *static_cast<float *>(mData) = f;
        },
        [](const TypedScopePtr &b) {
            std::terminate();
        },
        [this](const Matrix3 &m) {
            *static_cast<Matrix3 *>(mData) = m;
        },
        [this](const Matrix4 &m) {
            *static_cast<Matrix4 *>(mData) = m;
        },
        [this](const Quaternion &q) {
            *static_cast<Quaternion *>(mData) = q;
        },
        [this](const Vector4 &v) {
            *static_cast<Vector4 *>(mData) = v;
        },
        [this](const Vector3 &v) {
            *static_cast<Vector3 *>(mData) = v;
        },
        [this](const Vector2 &v) {
            *static_cast<Vector2 *>(mData) = v;
        },
        [this](const KeyValueVirtualIterator &it) {
            *static_cast<KeyValueVirtualIterator *>(mData) = it;
        },
        [this](const ApiMethod &m) {
            *static_cast<ApiMethod *>(mData) = m;
        },
        [this](const ObjectPtr &o) {
            *static_cast<ObjectPtr *>(mData) = o;
        } });

    return *this;
}

}

std::ostream &operator<<(std::ostream &stream,
    const Engine::ValueType &v)
{
    return stream << v.toString();
}
