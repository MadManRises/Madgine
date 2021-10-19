#pragma once

namespace Engine {

struct EnumMetaTable {

    constexpr bool isBase(int32_t value) const
    {
        return mBase && value < mBase->mMax;
    }

    constexpr std::string_view toString(int32_t value) const
    {
        if (isBase(value)) {
            return mBase->toString(value);
        } else {
            assert(value > mMin && value < mMax);
            return mValueNames[value - mMin - 1];
        }
    }

    constexpr bool fromString(std::string_view s, int32_t &v) const
    {
        const std::string_view *end = mValueNames + (mMax - mMin - 1);
        auto it = std::find(mValueNames, end, s);
        if (it != end) {
            v = (it - mValueNames) + mMin + 1;
            return true;
        } else if (mBase) {
            return mBase->fromString(s, v);
        } else {
            return false;
        }
    }

    std::ostream &print(std::ostream &stream, int32_t value, std::string_view actualType) const
    {
        if (isBase(value))
            return mBase->print(stream, value, actualType);
        return stream << actualType << "::" << toString(value) << " (" << value << ")";
    }

    const EnumMetaTable *mBase;
    std::string_view mName;
    const std::string_view *mValueNames;
    int32_t mMin, mMax;
};

template <typename _Representation>
struct EnumType : _Representation {

    using Representation = _Representation;
    using BaseType = typename _Representation::EnumType;

    EnumType() = default;

    EnumType(BaseType value)
        : mValue(value)
    {
    }

    operator BaseType() const
    {
        return mValue;
    }

    std::string_view toString() const
    {
        return Representation::sTable.toString(mValue);
    }

    bool fromString(std::string_view s)
    {
        return Representation::sTable.fromString(s, reinterpret_cast<int32_t &>(mValue));
        /*mValue = static_cast<EnumType>(v);
        return true;*/
    }

    friend std::ostream &operator<<(std::ostream &stream, const EnumType<Representation> &value)
    {
        return Representation::sTable.print(stream, value.mValue, sTypeName());
    }

    static std::string_view sTypeName()
    {
        return Representation::sTable.mName;
    }

protected:
    BaseType mValue;
};

template <typename _Representation, typename Base>
struct BaseEnum : EnumType<_Representation>, Base::Representation {
private:
    using EnumBase = EnumType<_Representation>;

public:
    using BaseType = typename EnumBase::BaseType;

    struct Representation : Base::Representation, EnumBase::Representation {
    };

    static constexpr int MIN = Base::MIN;
    static constexpr int MAX = EnumBase::MAX;
    static constexpr int COUNT = MAX - MIN - 1;

    using EnumBase::EnumBase;

    //TODO: using might be enough

    template <typename T>
    requires std::constructible_from<Base, T>
    BaseEnum(T v)
        : EnumBase(static_cast<BaseType>(static_cast<typename Base::BaseType>(v)))
    {
    }

    template <typename T>
    requires std::constructible_from<Base, T>
    bool operator==(T other) const
    {
        return toBase() == Base { other };
    }

    template <typename T>
    requires std::constructible_from<Base, T>
    bool operator!=(T other) const
    {
        return toBase() != Base { other };
    }

    std::string_view toString() const
    {
        return _Representation::sTable.toString(this->mValue);
    }

    friend std::ostream &operator<<(std::ostream &stream, const BaseEnum<_Representation, Base> &value)
    {
        return _Representation::sTable.print(stream, value.mValue, EnumBase::sTypeName());
    }

private:
    Base toBase() const
    {
        return { static_cast<typename Base::BaseType>(this->mValue) };
    }
};

}

#define ENUM_REGISTRY(Name, MIN_VAL, Base, ...)                                                                                                              \
    struct Name##Representation {                                                                                                                            \
        enum EnumType {                                                                                                                                      \
            MIN = MIN_VAL,                                                                                                                                   \
            __VA_ARGS__,                                                                                                                                     \
            MAX,                                                                                                                                             \
            COUNT = MAX - MIN - 1                                                                                                                            \
        };                                                                                                                                                   \
        static inline const constexpr auto sIdentifiers = Engine::StringUtil::tokenize<static_cast<size_t>(Name##Representation::COUNT)>(#__VA_ARGS__, ','); \
        static inline const constexpr Engine::EnumMetaTable sTable {                                                                                         \
            Base, #Name, sIdentifiers.data(), MIN, MAX                                                                                                       \
        };                                                                                                                                                   \
    };                                                                                                                                                       \
    inline std::ostream &operator<<(std::ostream &stream, typename Name##Representation::EnumType value)                                                     \
    {                                                                                                                                                        \
        return Name##Representation::sTable.print(stream, value, Name##Representation::sTable.mName);                                                        \
    }

#define ENUM_BASE(Name, Base, ...)                                                 \
    ENUM_REGISTRY(Name, Base::MAX - 1, &Base##Representation::sTable, __VA_ARGS__) \
    using Name = Engine::BaseEnum<Name##Representation, Base>;

#define ENUM(Name, ...)                           \
    ENUM_REGISTRY(Name, -1, nullptr, __VA_ARGS__) \
    using Name = Engine::EnumType<Name##Representation>;
