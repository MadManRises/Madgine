#pragma once

namespace Engine {

struct META_EXPORT EnumHolder {

    template <typename Rep>
    EnumHolder(Enum<Rep> e)
        : mValue(static_cast<int32_t>(e))
        , mTable(&Rep::sTable)
    {
    }

    template <typename Rep, typename Base>
    EnumHolder(BaseEnum<Rep, Base> e)
        : mValue(static_cast<int32_t>(e))
        , mTable(&Rep::sTable)
    {
    }

    bool operator==(const EnumHolder &other) const;
    
    const std::string_view &toString() const;
    const EnumMetaTable *table() const;

    int32_t value() const;
    void setValue(int32_t val);

    template <typename T>
    T safe_cast() const {
        if (mTable != &T::Representation::sTable)
            throw 0;
        return static_cast<typename T::Representation::EnumType>(mValue);
    }

private:
    int32_t mValue;
    const EnumMetaTable *mTable;
};

}