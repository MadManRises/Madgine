
#include "bits/array.h"

namespace Engine {

template <typename Enum>
struct Flags {

    using Representation = typename Enum::Representation;

    Flags() = default;

    Flags(BitArray<64> bits)
        : mBits(bits)
    {    
    }

    BitArray<64> values() const {
        return mBits;
    }

    friend std::ostream &operator<<(std::ostream &stream, const Flags<Enum> &value)
    {
        bool first = true;
        for (int32_t v : Representation::sTable.values<int32_t>()) {
            if (first)
                first = false;
            else
                stream << '|';
            Representation::sTable.print(stream, v, Representation::sTable.mName);
        }
        if (first)
            stream << '0';
        return stream;
    }

    friend std::istream &operator>>(std::istream &stream, Flags<Enum> &value)
    {
        std::string s;
        stream >> s;
        value.mBits = {};
        if (s != "0") {            
            for (std::string_view e : StringUtil::tokenize(s, '|')) {
                if (!e.starts_with(Representation::sTable.mName))
                    throw 0;
                e.remove_prefix(Representation::sTable.mName.size());
                if (!e.starts_with("::"))
                    throw 0;
                e.remove_prefix(2);
                int32_t v;
                if (!Representation::sTable.fromString(e, v))
                    throw 0;
                value.mBits[v] = true;
            }
        }
        return stream;
    }

private:
    BitArray<64> mBits;
};

}