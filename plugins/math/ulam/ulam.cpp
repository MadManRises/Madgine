#include "ulamlib.h"

#include "ulam.h"

namespace Engine {

Ulam::iterator Ulam::begin()
{
    return { mValue };
}

Ulam::iterator Ulam::end()
{
    return { 0 };
}

Ulam::iterator::iterator(int value)
    : mValue(value)
{
}

void Ulam::iterator::operator++()
{
    if (mValue == 1) {
        mValue = 0;
    } else if (mValue % 2 == 0) {
        ++mValue.mCount2;
        mValue.mValue /= 2;
    } else {
        ++mValue.mCount3;
        mValue.mValue = 3 * mValue.mValue + 1;
    }
}

bool Ulam::iterator::operator!=(const iterator &other) const
{
    return mValue != other.mValue;
}

const Ulam::Triple &Ulam::iterator::operator*() const
{
    return mValue;
}

}