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
    switch (index()) {
    case ValueTypeEnum::StringValue:
        switch (other.index()) {
        case ValueTypeEnum::StringValue:
            return std::get<CoWString>(mUnion) < std::get<CoWString>(other.mUnion);
        default:
            throw ValueTypeException(Database::Exceptions::invalidValueType);
        }
    case ValueTypeEnum::IntValue:
        switch (other.index()) {
        case ValueTypeEnum::IntValue:
            return std::get<int>(mUnion) < std::get<int>(other.mUnion);
        case ValueTypeEnum::FloatValue:
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
    switch (index()) {
    /*case Type::OwningStringValue:
        switch (other.type()) {
        case Type::OwningStringValue:
            *std::get<HeapObject<std::string>>(mUnion) += *std::get<HeapObject<std::string>>(other.mUnion);
            return;
        case Type::IntValue:
            *std::get<HeapObject<std::string>>(mUnion) += std::to_string(std::get<int>(other.mUnion));
        default:
            break;
        }
        break;*/
    case ValueTypeEnum::IntValue:
        switch (other.index()) {
        case ValueTypeEnum::IntValue:
            std::get<int>(mUnion) += std::get<int>(other.mUnion);
            return;
        case ValueTypeEnum::FloatValue:
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
    switch (index()) {
    case ValueTypeEnum::IntValue:
        switch (other.index()) {
        case ValueTypeEnum::IntValue:
            std::get<int>(mUnion) -= std::get<int>(other.mUnion);
            return;
        case ValueTypeEnum::FloatValue:
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
    switch (index()) {
    case ValueTypeEnum::IntValue:
        switch (other.index()) {
        case ValueTypeEnum::IntValue:
            std::get<int>(mUnion) /= std::get<int>(other.mUnion);
            return;
        case ValueTypeEnum::FloatValue:
            *this = std::get<int>(mUnion) / std::get<float>(other.mUnion);
        default:
            break;
        }
        break;
    case ValueTypeEnum::FloatValue:
        switch (other.index()) {
        case ValueTypeEnum::IntValue:
            std::get<float>(mUnion) /= std::get<int>(other.mUnion);
            return;
        case ValueTypeEnum::FloatValue:
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
    switch (index()) {
    case ValueTypeEnum::IntValue:
        switch (other.index()) {
        case ValueTypeEnum::IntValue:
            std::get<int>(mUnion) *= std::get<int>(other.mUnion);
            return;
        case ValueTypeEnum::FloatValue:
            *this = std::get<int>(mUnion) * std::get<float>(other.mUnion);
        default:
            break;
        }
        break;
    case ValueTypeEnum::FloatValue:
        switch (other.index()) {
        case ValueTypeEnum::IntValue:
            std::get<float>(mUnion) *= std::get<int>(other.mUnion);
            return;
        case ValueTypeEnum::FloatValue:
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
    switch (index()) {
    case ValueTypeEnum::BoolValue:
        return std::get<bool>(mUnion) ? "true" : "false";
    case ValueTypeEnum::StringValue:
        return "\""s + std::string { std::get<CoWString>(mUnion) } + "\"";
    case ValueTypeEnum::IntValue:
        return std::to_string(std::get<int>(mUnion));
    case ValueTypeEnum::UIntValue:
        return std::to_string(std::get<uint64_t>(mUnion));
    case ValueTypeEnum::NullValue:
        return "NULL";
    case ValueTypeEnum::ScopeValue:
        return std::get<TypedScopePtr>(mUnion).name();
    case ValueTypeEnum::FloatValue:
        return std::to_string(std::get<float>(mUnion));
    case ValueTypeEnum::Vector2Value:
        return "["s + std::to_string(std::get<Vector2>(mUnion).x) + ", " + std::to_string(std::get<Vector2>(mUnion).y);
    case ValueTypeEnum::Vector3Value:
        return "["s + std::to_string(std::get<Vector3>(mUnion).x) + ", " + std::to_string(std::get<Vector3>(mUnion).y) + ", " + std::to_string(std::get<Vector3>(mUnion).z) + "]";
    case ValueTypeEnum::Vector4Value:
        return "["s + std::to_string(std::get<Vector4>(mUnion)[0]) + ", " + std::to_string(std::get<Vector4>(mUnion)[1]) + ", " + std::to_string(std::get<Vector4>(mUnion)[2]) + ", " + std::to_string(std::get<Vector4>(mUnion)[3]) + "]";
    case ValueTypeEnum::QuaternionValue:
        return "{"s + std::to_string(std::get<Quaternion>(mUnion).v.x) + ", " + std::to_string(std::get<Quaternion>(mUnion).v.y) + ", " + std::to_string(std::get<Quaternion>(mUnion).v.z) + ", " + std::to_string(std::get<Quaternion>(mUnion).w) + "}";
    case ValueTypeEnum::Matrix3Value:
        return "[ ["s + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[0][0]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[0][1]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[0][2]) + "], [" + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[1][0]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[1][1]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[1][2]) + "], [" + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[2][0]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[2][1]) + ", " + std::to_string((*std::get<CoW<Matrix3>>(mUnion))[2][2]) + "] ]";
    case ValueTypeEnum::Matrix4Value:
        return "[ ["s + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[0][0]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[0][1]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[0][2]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[0][3]) + "], [" + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[1][0]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[1][1]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[1][2]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[1][3]) + "], [" + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[2][0]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[2][1]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[2][2]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[2][3]) + "], [" + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[3][0]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[3][1]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[3][2]) + ", " + std::to_string((*std::get<CoW<Matrix4>>(mUnion))[3][3]) + "] ]";
    case ValueTypeEnum::ApiFunctionValue:
        return "<method>";
    default:
        throw ValueTypeException("Unknown Type!");
    }
}

std::string ValueType::toShortString() const
{
    switch (index()) {
    case ValueTypeEnum::BoolValue:
        return std::get<bool>(mUnion) ? "true" : "false";
    case ValueTypeEnum::StringValue:
        return "\""s + std::string { std::get<CoWString>(mUnion) } + "\"";
    case ValueTypeEnum::IntValue:
        return std::to_string(std::get<int>(mUnion));
    case ValueTypeEnum::UIntValue:
        return std::to_string(std::get<uint64_t>(mUnion));
    case ValueTypeEnum::NullValue:
        return "NULL";
    case ValueTypeEnum::ScopeValue:
        return std::get<TypedScopePtr>(mUnion).name();
    case ValueTypeEnum::FloatValue:
        return std::to_string(std::get<float>(mUnion));
    case ValueTypeEnum::Vector2Value:
        return "["s + std::to_string(std::get<Vector2>(mUnion).x) + ", " + std::to_string(std::get<Vector2>(mUnion).y) + "]";
    case ValueTypeEnum::Vector3Value:
        return "["s + std::to_string(std::get<Vector3>(mUnion).x) + ", " + std::to_string(std::get<Vector3>(mUnion).y) + ", " + std::to_string(std::get<Vector3>(mUnion).z) + "]";
    case ValueTypeEnum::Vector4Value:
        return "["s + std::to_string(std::get<Vector4>(mUnion)[0]) + ", " + std::to_string(std::get<Vector4>(mUnion)[1]) + ", " + std::to_string(std::get<Vector4>(mUnion)[2]) + ", " + std::to_string(std::get<Vector4>(mUnion)[3]) + "]";
    case ValueTypeEnum::QuaternionValue:
        return "{"s + std::to_string(std::get<Quaternion>(mUnion).v.x) + ", " + std::to_string(std::get<Quaternion>(mUnion).v.y) + ", " + std::to_string(std::get<Quaternion>(mUnion).v.z) + ", " + std::to_string(std::get<Quaternion>(mUnion).w) + "}";
    case ValueTypeEnum::Matrix3Value:
        return "Matrix3[...]";
    case ValueTypeEnum::Matrix4Value:
        return "Matrix4[...]";
    case ValueTypeEnum::ApiFunctionValue:
        return "<method>";
    default:
        throw ValueTypeException("Unknown Type!");
    }
}

std::string ValueType::getTypeString() const
{
    return type().toString();
}

ValueTypeDesc ValueType::type() const
{
    ValueTypeIndex i = index();
    switch (i) {
    case ValueTypeEnum::ScopeValue:
        return { i, as<TypedScopePtr>().mType->mSelf };
    default:
        return { i };
    }
}

ValueTypeIndex ValueType::index() const
{
    return static_cast<ValueTypeEnum>(mUnion.index());
}

template <size_t... Is>
static void setTypeHelper(ValueType::Union &v, ValueTypeDesc type, std::index_sequence<Is...>)
{
    using Ctor_Type = void (*)(ValueType::Union &);
    static constexpr Ctor_Type ctors[] = {
        [](ValueType::Union &v) {
            if constexpr (!type_pack_contains_v<ValueType::NonDefaultConstructibleTypes, std::variant_alternative_t<Is, ValueType::Union>>)
                v.emplace<Is>();
            else
                throw 0;
        }...
    };
    ctors[static_cast<unsigned char>(type.mType.mIndex)](v);
}

void ValueType::setType(ValueTypeDesc type)
{
    if (this->type() != type) {
        setTypeHelper(mUnion, type, std::make_index_sequence<std::variant_size_v<Union>>());
    }
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
    if (!v.type().canAccept(mValue.type()))
        std::terminate();

    mValue = v;
    mValue.visit(overloaded {
#define VALUETYPE_SEP ,
#define VALUETYPE_TYPE(Name, Storage, T, ...)       \
    [this](T t) {                                   \
        *static_cast<std::decay_t<T> *>(mData) = t; \
    }
#include "valuetypedef.h"
    });

    return *this;
}

ValueTypeRef &ValueTypeRef::operator=(ValueTypeRef &&other)
{
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
