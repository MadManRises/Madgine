#include "../moduleslib.h"
#include "valuetype.h"
#include "virtualscopebase.h"

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
            throw ValueTypeException("Invalid types for operator<");
        }
    case ValueTypeEnum::IntValue:
        switch (other.index()) {
        case ValueTypeEnum::IntValue:
            return std::get<int>(mUnion) < std::get<int>(other.mUnion);
        case ValueTypeEnum::FloatValue:
            return std::get<int>(mUnion) < std::get<float>(other.mUnion);
        default:
            throw ValueTypeException("Invalid types for operator<");
        }
    default:
        throw ValueTypeException("Invalid types for operator<");
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
        throw ValueTypeException("Invalid types for operator+");
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
        throw ValueTypeException("Invalid types for operator-");
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
        throw ValueTypeException("Invalid types for operator/");
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
        throw ValueTypeException("Invalid types for operator*");
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
    return visit(overloaded {
        [](bool b) {
            return b ? "true"s : "false"s;
        },
        [](const CoWString &s) {
            return "\"" + std::string { s } + "\"";
        },
        [](std::monostate) {
            return "NULL"s;
        },
        [](const TypedScopePtr &scope) {
            return scope.name();
        },
        [](const std::shared_ptr<VirtualScopeBase> &scope) {
            return scope->customScopePtr().name();
        },
        [](const Vector2 &v) {
            return "["s + std::to_string(v.x) + ", " + std::to_string(v.y) + "]";
        },
        [](const Vector3 &v) {
            return "["s + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + "]";
        },
        [](const Vector4 &v) {
            return "["s + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + "]";
        },
        [](const Quaternion &q) {
            return "{"s + std::to_string(q.v.x) + ", " + std::to_string(q.v.y) + ", " + std::to_string(q.v.z) + ", " + std::to_string(q.w) + "}";
        },
        [](const CoW<Matrix3> &m) {
            return "[ ["s + std::to_string((*m)[0][0]) + ", " + std::to_string((*m)[0][1]) + ", " + std::to_string((*m)[0][2]) + "], [" + std::to_string((*m)[1][0]) + ", " + std::to_string((*m)[1][1]) + ", " + std::to_string((*m)[1][2]) + "], [" + std::to_string((*m)[2][0]) + ", " + std::to_string((*m)[2][1]) + ", " + std::to_string((*m)[2][2]) + "] ]";
        },
        [](const CoW<Matrix4> &m) {
            return "[ ["s + std::to_string((*m)[0][0]) + ", " + std::to_string((*m)[0][1]) + ", " + std::to_string((*m)[0][2]) + ", " + std::to_string((*m)[0][3]) + "], [" + std::to_string((*m)[1][0]) + ", " + std::to_string((*m)[1][1]) + ", " + std::to_string((*m)[1][2]) + ", " + std::to_string((*m)[1][3]) + "], [" + std::to_string((*m)[2][0]) + ", " + std::to_string((*m)[2][1]) + ", " + std::to_string((*m)[2][2]) + ", " + std::to_string((*m)[2][3]) + "], [" + std::to_string((*m)[3][0]) + ", " + std::to_string((*m)[3][1]) + ", " + std::to_string((*m)[3][2]) + ", " + std::to_string((*m)[3][3]) + "] ]";
        },
        [](const ApiFunction &) {
            return "<function>"s;
        },
        [](const KeyValueVirtualRange &) {
            return "<range>"s;
        },
        [](const ObjectPtr &) {
            return "<object>"s;
        },
        [](const auto &v) {
            return std::to_string(v);
        } });
}

std::string ValueType::toShortString() const
{
    return visit(overloaded {
        [](bool b) {
            return b ? "true"s : "false"s;
        },
        [](const CoWString &s) {
            return "\"" + std::string { s } + "\"";
        },
        [](std::monostate) {
            return "NULL"s;
        },
        [](const TypedScopePtr &scope) {
            return scope.name();
        },
        [](const std::shared_ptr<VirtualScopeBase> &scope) {
            return scope->customScopePtr().name();
        },
        [](const Vector2 &v) {
            return "["s + std::to_string(v.x) + ", " + std::to_string(v.y) + "]";
        },
        [](const Vector3 &v) {
            return "["s + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + "]";
        },
        [](const Vector4 &v) {
            return "["s + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + "]";
        },
        [](const Quaternion &q) {
            return "{"s + std::to_string(q.v.x) + ", " + std::to_string(q.v.y) + ", " + std::to_string(q.v.z) + ", " + std::to_string(q.w) + "}";
        },
        [](const CoW<Matrix3> &) {
            return "Matrix3[...]"s;
        },
        [](const CoW<Matrix4> &) {
            return "Matrix4[...]"s;
        },
        [](const ApiFunction &) {
            return "<function>"s;
        },
        [](const KeyValueVirtualRange &) {
            return "<range>"s;
        },
        [](const ObjectPtr &) {
            return "<object>"s;
        },
        [](const auto &v) {
            return std::to_string(v);
        } });
}

std::string ValueType::getTypeString() const
{
    return type().toString();
}

bool ValueType::isReference() const
{
    return visit(overloaded {
        [](const TypedScopePtr &) {
            return true;
        },
        [](const KeyValueVirtualRange &range) {
            return range.isReference();
        },
        [](const auto &) {
            return false;
        } });
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
        if (type.mType == ValueTypeEnum::ScopeValue) {
            std::get<TypedScopePtr>(mUnion).mType = *type.mSecondary.mMetaTable;
        } else if (type.mType == ValueTypeEnum::ApiFunctionValue) {
            std::get<ApiFunction>(mUnion).mTable = *type.mSecondary.mFunctionTable;
        }
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
