#pragma once

#include "stringutil.h"

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
struct Enum : _Representation {

    using Representation = _Representation;
    using EnumType = typename _Representation::EnumType;

    Enum() = default;

    Enum(EnumType value)
        : mValue(value)
    {
    }

    operator EnumType() const
    {
        return mValue;
    }

    std::string_view toString() const
    {
        return Representation::sTable.toString(mValue);
    }

    friend std::ostream &operator<<(std::ostream &stream, const Enum<Representation> &value)
    {
        return Representation::sTable.print(stream, value.mValue, sTypeName());
    }

    static std::string_view sTypeName()
    {
        return Representation::sTable.mName;
    }

protected:
    EnumType mValue;
};

template <typename _Representation, typename Base>
struct BaseEnum : Enum<_Representation>, Base::Representation {
private:
    using EnumBase = Enum<_Representation>;

public:
    using EnumType = typename EnumBase::EnumType;

    struct Representation : Base::Representation, EnumBase::Representation {
    };

    static constexpr int MIN = Base::MIN;
    static constexpr int MAX = EnumBase::MAX;
    static constexpr int COUNT = MAX - MIN - 1;

    using EnumBase::EnumBase;

    //TODO: using might be enough

    template <typename T, typename = std::enable_if_t<std::is_constructible_v<Base, T>>>
    BaseEnum(T v)
        : EnumBase(static_cast<EnumType>(static_cast<typename Base::EnumType>(v)))
    {
    }

    template <typename T, typename = std::enable_if_t<std::is_constructible_v<Base, T>>>
    bool operator==(T other) const
    {
        return toBase() == Base { other };
    }

    template <typename T, typename = std::enable_if_t<std::is_constructible_v<Base, T>>>
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
        return { static_cast<typename Base::EnumType>(this->mValue) };
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
    using Name = Engine::Enum<Name##Representation>;
