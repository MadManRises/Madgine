#pragma once

#include "proxyscopebase.h"

namespace Engine {

struct META_EXPORT OwnedScopePtr {

    OwnedScopePtr() = default;
    OwnedScopePtr(const OwnedScopePtr &) = default;
    //OwnedScopePtr(OwnedScopePtr &) = default;
    OwnedScopePtr(OwnedScopePtr &&) = default;
    OwnedScopePtr(std::shared_ptr<ProxyScopeBase> ptr);

    OwnedScopePtr &operator=(const OwnedScopePtr &) = default;
    OwnedScopePtr &operator=(OwnedScopePtr &&) = default;

    template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, OwnedScopePtr>>>
    explicit OwnedScopePtr(T &&t)
        : mScope(std::make_shared<ScopeWrapper<std::remove_reference_t<T>>>(std::forward<T>(t)))
    {
    }

    template <typename T, typename = std::enable_if_t<!std::is_same_v<std::decay_t<T>, OwnedScopePtr>>>
    OwnedScopePtr &operator=(T &&t)
    {
        mScope = std::make_shared<ScopeWrapper<std::remove_reference_t<T>>>(std::forward<T>(t));
        return *this;
    }

    std::string name() const;
    const MetaTable *type() const;

    bool operator==(const OwnedScopePtr &other) const;

    TypedScopePtr get() const;

    template <typename T>
    T &safe_cast() const
    {
        return *get().safe_cast<T>();
    }

private:
    std::shared_ptr<ProxyScopeBase> mScope;
};

}