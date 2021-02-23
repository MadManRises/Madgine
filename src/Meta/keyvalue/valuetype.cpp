#include "../metalib.h"
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
        [](const OwnedScopePtr &scope) {
            return scope.name();
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
            if constexpr (std::is_default_constructible_v<std::variant_alternative_t<Is, ValueType::Union>>)
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
#include "valuetypedefinclude.h"
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

DERIVE_OPERATOR(StreamOut, <<)

std::ostream &operator<<(std::ostream &stream,
    const Engine::ValueType &v)
{
    stream << "<" << v.getTypeString() << ">";

    v.visit([&](const auto &v) {
        if constexpr (has_operator_StreamOut_v<decltype(v), std::ostream &> || has_operator_StreamOut_v<std::ostream &, decltype(v)>)
            stream << std::forward<decltype(v)>(v);
    });

    return stream;
}
