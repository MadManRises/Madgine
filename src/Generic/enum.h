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
        return stream << actualType << "::" << toString(value);
    }

    std::istream &read(std::istream &stream, int32_t &value, std::string_view actualType) const
    {
        std::istream::pos_type p = stream.tellg();
        std::string buffer;
        buffer.resize(actualType.size());
        if (!stream.read(buffer.data(), actualType.size())) {
            stream.seekg(p);
            return stream;
        }
        if (actualType != buffer) {
            stream.seekg(p);
            stream.setstate(std::ios_base::failbit);
            return stream;
        }
        if (!stream.read(buffer.data(), 2)) {
            stream.seekg(p);
            return stream;
        }
        if (buffer[0] != ':' || buffer[1] != ':') {
            stream.seekg(p);
            stream.setstate(std::ios_base::failbit);
            return stream;
        }
        if (!(stream >> buffer)) {
            stream.seekg(p);
            return stream;
        }
        if (!fromString(buffer, value)) {
            stream.seekg(p);
            stream.setstate(std::ios_base::failbit);
        }
        return stream;
    }

    template <typename T>
    Generator<T> values() const {
        int32_t val = mMin + 1;
        while (val < mMax) {
            co_yield static_cast<T>(val);
            ++val;
        }
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

    friend std::istream &operator>>(std::istream &stream, EnumType<Representation> &value)
    {
        int32_t dummy;
        if (Representation::sTable.read(stream, dummy, sTypeName()))
            value.mValue = static_cast<BaseType>(dummy);
        return stream;
    }

    static std::string_view sTypeName()
    {
        return Representation::sTable.mName;
    }

    static Generator<BaseType> values() {
        return Representation::sTable.template values<BaseType>();
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
    requires std::constructible_from<Base, T> bool operator==(T other) const
    {
        return toBase() == Base { other };
    }

    template <typename T>
    requires std::constructible_from<Base, T> bool operator!=(T other) const
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

    friend std::istream &operator>>(std::istream &stream, BaseEnum<_Representation, Base> &value)
    {
        return _Representation::sTable.read(stream, value.mValue, EnumBase::sTypeName());
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
    }                                                                                                                                                        \
    inline std::istream &operator>>(std::istream &stream, typename Name##Representation::EnumType &value)                                                    \
    {                                                                                                                                                        \
        int32_t dummy;                                                                                                                                       \
        if (Name##Representation::sTable.read(stream, dummy, Name##Representation::sTable.mName))                                                            \
            value = static_cast<typename Name##Representation::EnumType>(dummy);                                                                             \
        return stream;                                                                                                                                       \
    }

#define ENUM_BASE(Name, Base, ...)                                                 \
    ENUM_REGISTRY(Name, Base::MAX - 1, &Base##Representation::sTable, __VA_ARGS__) \
    using Name = Engine::BaseEnum<Name##Representation, Base>;

#define ENUM(Name, ...)                           \
    ENUM_REGISTRY(Name, -1, nullptr, __VA_ARGS__) \
    using Name = Engine::EnumType<Name##Representation>;

#define FORWARD_ENUM(Name)       \
    struct Name##Representation; \
    using Name = Engine::EnumType<Name##Representation>;
