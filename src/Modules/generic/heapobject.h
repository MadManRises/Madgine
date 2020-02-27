#pragma once

namespace Engine {

template <typename T>
struct HeapObject {

    template <typename... Args>
    HeapObject(Args &&... args)
        : mObject(std::make_unique<T>(std::forward<Args>(args)...))
    {
    }

    HeapObject(const HeapObject<T> &other)
        : mObject(std::make_unique<T>(*other))
    {
    }

	HeapObject(HeapObject<T> &&) = default;

    HeapObject<T> &operator=(const HeapObject<T> &other)
    {
        *mObject = *other;
        return *this;
    }

    bool operator==(const HeapObject<T> &other) const
    {
        return *mObject == *other;
    }

    operator T &()
    {
        return *mObject;
    }

    operator const T &() const
    {
        return *mObject;
    }

    T &operator*()
    {
        return *mObject;
    }

    const T &operator*() const
    {
        return *mObject;
    }

    T *operator->()
    {
        return mObject.get();
    }

    const T *operator->() const
    {
        return mObject.get();
    }

private:
    std::unique_ptr<T> mObject;
};

}