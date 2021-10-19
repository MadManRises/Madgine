#pragma once

namespace Engine {
namespace Threading {

	enum class AccessMode {
        READ,
        WRITE
    };

    struct MODULES_EXPORT DataMutex {
		
        enum ConsiderResult {
            UNAVAILABLE,
            ALREADY_CONSIDERED,
            CONSIDERED
        };

        bool isAvailable(AccessMode mode) const;
        ConsiderResult consider(AccessMode mode);
        void unconsider();

        bool lock(AccessMode mode);
        void unlock(AccessMode mode);

    private:
        std::atomic<std::thread::id> mCurrentHolder = std::thread::id {};
        std::atomic<unsigned short> mReaderCount = 0;
        std::atomic_flag mConsidered = ATOMIC_FLAG_INIT;
    };

    struct MODULES_EXPORT DataLock {
        DataLock(DataMutex &mutex, AccessMode mode);
        DataLock(const DataLock &) = delete;
        DataLock(DataLock &&other);
        ~DataLock();

    private:
        DataMutex &mMutex;
        bool mHoldsLock;
        AccessMode mMode;
    };

    template <size_t S>
    bool try_lockData(std::array<DataMutex *, S> data, AccessMode mode)
    {
        std::sort(data.begin(), data.end());
        return try_lockData_sorted(data.begin(), data.end(), mode);
    }
    bool try_lockData_sorted(DataMutex *const *begin, DataMutex *const *end, AccessMode mode);

}
}