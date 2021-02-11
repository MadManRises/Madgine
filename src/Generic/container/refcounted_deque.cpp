#include "../genericlib.h"

#include "refcounted_deque.h"

namespace Engine {

ControlBlockBase::ControlBlockBase()
    : mDeadFlag(false)
    , mRefCount(0)
{
}

void ControlBlockBase::incRef()
{
}

void ControlBlockBase::decRef()
{
}

bool ControlBlockBase::dead() const
{
    return mDeadFlag;
}

}