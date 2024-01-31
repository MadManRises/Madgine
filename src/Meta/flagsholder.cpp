#include "metalib.h"

#include "flagsholder.h"

#include "Generic/enum.h"

namespace Engine {

bool FlagsHolder::operator==(const FlagsHolder &other) const
{
    if (mTable != other.mTable)
        return false;
    if (!mTable)
        return true;
    return mValue == other.mValue;
}

const EnumMetaTable *FlagsHolder::table() const
{
    return mTable;
}

std::ostream &operator<<(std::ostream &stream, const FlagsHolder &value)
{
    throw 0;
    //return value.mTable->print(stream, value.mValue, value.mTable->mName);
}

std::istream &operator>>(std::istream &stream, FlagsHolder &value)
{
    throw 0;
    //return value.mTable->read(stream, value.mValue, value.mTable->mName);
}

}