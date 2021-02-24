#pragma once

namespace Engine {

template <typename T, typename Holder = std::unique_ptr<T>>
struct HeapMember {

    template <typename... Args>
    HeapMember(Args &&... args)
        : mObject(std::make_unique<T>(std::forward<Args>(args)...))
    {
    }

    HeapMember(Holder holder)
        : mObject(std::move(holder))
    {
    }

    HeapMember(const HeapMember<T, Holder> &other)
        : mObject(std::make_unique<T>(*other))
    {
    }

	HeapMember(HeapMember<T, Holder> &&) = default;

    HeapMember<T, Holder> &operator=(const HeapMember<T, Holder> &other)
    {
        *mObject = *other;
        return *this;
    }

    bool operator==(const HeapMember<T, Holder> &other) const
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
    Holder mObject;
};

}