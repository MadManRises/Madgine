#include "../moduleslib.h"

#include "datamutex.h"

namespace Engine {
namespace Threading {

    bool DataMutex::isHeldWrite() const
    {
        return mCurrentHolder == std::this_thread::get_id();
    }

    bool DataMutex::isAvailable(AccessMode mode) const
    {
        std::thread::id holder = mCurrentHolder;
        if (mode == AccessMode::READ) {
            return holder == std::thread::id {} || holder == std::this_thread::get_id();
        } else {
            return (holder == std::thread::id {} && mReaderCount == 0) || holder == std::this_thread::get_id();
        }
    }

    DataMutex::ConsiderResult DataMutex::consider(AccessMode mode)
    {
        bool alreadyConsidered = mConsidered.test_and_set();
        if (!isAvailable(mode)) {
            if (!alreadyConsidered)
                mConsidered.clear();
            return UNAVAILABLE;
        } else {
            return alreadyConsidered ? ALREADY_CONSIDERED : CONSIDERED;
        }
    }

    void DataMutex::unconsider()
    {
        mConsidered.clear();
    }

    bool DataMutex::lock(AccessMode mode)
    {
        while (consider(mode) != CONSIDERED) //TODO: rewrite to us wait
            ;
        bool result = true;
        if (mode == AccessMode::READ) {
            if (mCurrentHolder == std::thread::id {}) {
                ++mReaderCount;
            } else {
                assert(mCurrentHolder == std::this_thread::get_id());
                result = false;
            }
        } else {
            std::thread::id expected = {};
            std::thread::id self = std::this_thread::get_id();
            if (!mCurrentHolder.compare_exchange_strong(expected, self)) {
                assert(expected == self);
            }
            result = expected != self;
        }
        unconsider();
        return result;
    }

    void DataMutex::unlock(AccessMode mode)
    {
        if (mode == AccessMode::READ) {
            --mReaderCount;
        } else {
            std::thread::id expected = std::this_thread::get_id();
            if (!mCurrentHolder.compare_exchange_strong(expected, {}))
                std::terminate();
        }
    }

    DataLock::DataLock(DataMutex &mutex, AccessMode mode)
        : mMutex(mutex)
        , mHoldsLock(mutex.lock(mode))
        , mMode(mode)
    {
    }

    DataLock::DataLock(DataLock &&other)
        : mMutex(other.mMutex)
        , mHoldsLock(std::exchange(other.mHoldsLock, false))
        , mMode(other.mMode)
    {
    }

    DataLock::~DataLock()
    {
        if (mHoldsLock)
            mMutex.unlock(mMode);
    }

    bool try_lockData_sorted(DataMutex **begin, DataMutex **end, AccessMode mode)
    {
        DataMutex **it = begin;

        bool success = true;
        bool checkAvailability = true;

        while (it != end && success) {
            if (checkAvailability) {
                checkAvailability = false;
                for (DataMutex **check = it; check != end && success; ++check) {
                    if (!(*check)->isAvailable(mode)) {
                        success = false;
                    }
                }
            }

            if (success) {
                switch ((*it)->consider(mode)) {
                case DataMutex::UNAVAILABLE:
                    success = false;
                    break;
                case DataMutex::ALREADY_CONSIDERED:
                    checkAvailability = true;
                    break;
                case DataMutex::CONSIDERED:
                    ++it;
                    break;
                }
            }
        }

        //cleanup
        while (it != begin) {
            --it;
            (*it)->unconsider();
        }

        return success;
    }

}
}
