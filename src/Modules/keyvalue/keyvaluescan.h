#pragma once

#include "typedscopeptr.h"

namespace Engine {

template <typename F>
void kvScan(F &&f, const TypedScopePtr &scope, std::string_view name = {}, TypedScopePtr parent = {})
{
    if constexpr (std::is_invocable_v<F, TypedScopePtr, std::string_view, TypedScopePtr>) {
        f(scope, name, parent);
    }

    for (ScopeField field : scope) {
        field.value().visit([&](auto &&v) { kvScan(f, std::forward<decltype(v)>(v), field.key(), scope); });
    }
}

template <typename F>
void kvScan(F&& f, KeyValueVirtualIterator it, std::string_view name = {}, TypedScopePtr parent = {}) {
    if constexpr (std::is_invocable_v<F, KeyValueVirtualIterator, std::string_view, TypedScopePtr>) {
        f(it, name, parent);
    }

    for (; it != VirtualIteratorEnd {}; ++it) {
        it->mValue.value().visit([&](auto &&v) { kvScan(f, std::forward<decltype(v)>(v), name, parent); });        
    }
}    
    
template <typename F, typename T>
void kvScan(F &&f, const T &t, std::string_view name = {}, TypedScopePtr parent = {})
{
    if constexpr (std::is_invocable_v<F, T, std::string_view, TypedScopePtr>) {
        f(t, name, parent);
    }
}

}