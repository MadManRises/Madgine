#pragma once

namespace Engine {

template <typename T>
struct HeapMember {

    template <typename... Args>
    HeapMember(Args &&... args)
        : mObject(std::make_unique<T>(std::forward<Args>(args)...))
    {
    }

    HeapMember(const HeapMember<T> &other)
        : mObject(std::make_unique<T>(*other))
    {
    }

	HeapMember(HeapMember<T> &&) = default;

    HeapMember<T> &operator=(const HeapMember<T> &other)
    {
        *mObject = *other;
        return *this;
    }

    bool operator==(const HeapMember<T> &other) const
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