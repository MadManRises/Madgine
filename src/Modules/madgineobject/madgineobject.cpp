#include "../moduleslib.h"
#include "madgineobject.h"

namespace Engine {

class DependencyInitException {
};

bool MadgineObject::isInitialized() const
{
    return mState == ObjectState::INITIALIZED || mState == ObjectState::MARKED_INITIALIZED;
}

MadgineObject::MadgineObject()
    : mState(ObjectState::UNINITIALIZED)
    , mOrder(-1)
{
}

MadgineObject::~MadgineObject()
{
    if (mState != ObjectState::UNINITIALIZED) {
        LOG_WARNING("Deleting still initialized Object: "s + mName);
    }
}

ObjectState MadgineObject::getState() const
{
    return mState;
}

bool MadgineObject::callInit()
{
    if (mState == ObjectState::UNINITIALIZED) {
        mState = ObjectState::INITIALIZING;
        mName = typeid(*this).name();
        bool result;
        try {
            LOG("Initializing: " << mName << "...");
            result = init();
        } catch (const DependencyInitException &) {
            result = false;
        }
        if (result)
            mState = ObjectState::INITIALIZED;
        else if (mState == ObjectState::INITIALIZING)
            mState = ObjectState::UNINITIALIZED;
        else if (mState == ObjectState::MARKED_INITIALIZED) {
            mState = ObjectState::INITIALIZED;
            callFinalize();
        } else
            std::terminate();
        LOG((isInitialized() ? "Success" : "Failure"));
    }
    return isInitialized();
}

bool MadgineObject::callInit(int &count)
{
    bool notInit = mState == ObjectState::UNINITIALIZED;
    bool result = callInit();
    if (notInit && mState == ObjectState::INITIALIZED)
        mOrder = ++count;
    return result;
}

void MadgineObject::callFinalize()
{
    if (mState == ObjectState::INITIALIZED) {
        finalize();
        mState = ObjectState::UNINITIALIZED;
    }
    assert(mState == ObjectState::UNINITIALIZED);
}

void MadgineObject::callFinalize(int order)
{
    if (mOrder == order)
        callFinalize();
}

void MadgineObject::checkInitState() const
{
    if (mState == ObjectState::UNINITIALIZED)
        throw DependencyInitException {};
}

void MadgineObject::checkDependency() const
{
    switch (mState) {
    case ObjectState::UNINITIALIZED:
        throw DependencyInitException {};
    case ObjectState::INITIALIZING:
        LOG_WARNING("Possible circular dependency! Consider using markInitialized!");
        break;
    default:
        break;
    }
}

void MadgineObject::markInitialized()
{
    assert(mState == ObjectState::INITIALIZING);
    mState = ObjectState::MARKED_INITIALIZED;
}

}
