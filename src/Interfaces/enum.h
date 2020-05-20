#pragma once

#include "stringutil.h"

namespace Engine {

template <typename _Representation, typename Registry>
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

    static const std::string_view &toString(EnumType value)
    {
        assert(value > Representation::MIN && value < Representation::MAX);
        return Registry::sIdentifiers[value - Representation::MIN - 1];
    }

    const std::string_view &toString() const
    {
        return toString(mValue);
    }

    std::ostream &print(std::ostream &stream, const std::string_view &actualType) const
    {
        return stream << actualType << "::" << toString() << " (" << static_cast<int>(mValue) << ")";
    }

    friend std::ostream &operator<<(std::ostream &stream, const Enum<Representation, Registry> &value)
    {
        return value.print(stream, sTypeName());
    }

    static const std::string_view &sTypeName()
    {
        return Registry::sName;
    }

protected:
    EnumType mValue;
};

template <typename _Representation, typename Registry, typename Base>
struct BaseEnum : Enum<_Representation, Registry>, Base::Representation {
private:
    using EnumBase = Enum<_Representation, Registry>;

public:
    using EnumType = typename EnumBase::EnumType;

    struct Representation : Base::Representation, EnumBase::Representation {
    };

    static constexpr int MIN = Base::MIN;
    static constexpr int MAX = EnumBase::MAX;
    static constexpr int COUNT = MAX - MIN - 1;

    using EnumBase::EnumBase;

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

    const std::string_view &toString() const
    {
        if (isBase())
            return toBase().toString();
        return EnumBase::toString();
    }

    std::ostream &print(std::ostream &stream, const std::string_view &actualType) const
    {
        if (isBase())
            return toBase().print(stream, actualType);
        return EnumBase::print(stream, actualType);
    }

    friend std::ostream &operator<<(std::ostream &stream, const BaseEnum<_Representation, Registry, Base> &value)
    {
        return value.print(stream, EnumBase::sTypeName());
    }

private:
    Base toBase() const
    {
        return { static_cast<typename Base::EnumType>(this->mValue) };
    }

    bool isBase() const
    {
        assert(static_cast<int>(this->mValue) > MIN);
        return static_cast<int>(this->mValue) < static_cast<int>(Base::MAX);
    }
};

}

#define ENUM_REGISTRY(Name, MIN_VAL, ...)                                                                                                                    \
    struct Name##Representation {                                                                                                                            \
        enum EnumType {                                                                                                                                      \
            MIN = MIN_VAL,                                                                                                                                   \
            __VA_ARGS__,                                                                                                                                     \
            MAX,                                                                                                                                             \
            COUNT = MAX - MIN - 1                                                                                                                            \
        };                                                                                                                                                   \
    };                                                                                                                                                       \
    struct Name##Registry {                                                                                                                                  \
        static inline const constexpr std::string_view sName = #Name;                                                                                        \
        static inline const constexpr auto sIdentifiers = Engine::StringUtil::tokenize<static_cast<size_t>(Name##Representation::COUNT)>(#__VA_ARGS__, ','); \
    };                                                                                                                                                       \
    inline std::ostream &operator<<(std::ostream &stream, typename Name##Representation::EnumType value)                                                     \
    {                                                                                                                                                        \
        return stream << Engine::Enum<Name##Representation, Name##Registry> { value };                                                                       \
    }

#define ENUM_BASE(Name, Base, ...)                  \
    ENUM_REGISTRY(Name, Base::MAX - 1, __VA_ARGS__) \
    using Name = Engine::BaseEnum<Name##Representation, Name##Registry, Base>;

#define ENUM(Name, ...)                  \
    ENUM_REGISTRY(Name, -1, __VA_ARGS__) \
    using Name = Engine::Enum<Name##Representation, Name##Registry>;
