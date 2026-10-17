#pragma once

#include "Generic/execution/concepts.h"
#include "Generic/execution/query.h"
#include "Generic/typed_ptr.h"

namespace Engine {
namespace Debug {

    struct get_debug_context_t {

        using signature = ContextInfo &();

        template <typename T>
            requires tag_invocable<get_debug_context_t, T &>
        auto operator()(T &t) const
            noexcept(is_nothrow_tag_invocable_v<get_debug_context_t, T &>)
                -> tag_invoke_result_t<get_debug_context_t, T &>
        {
            return tag_invoke(*this, t);
        }
    };

    inline constexpr get_debug_context_t get_debug_context;

    struct get_debug_child_location_t {

        using signature = TypedPtr &();

        template <typename T>
            requires tag_invocable<get_debug_child_location_t, T &>
        auto operator()(T &t) const
            noexcept(is_nothrow_tag_invocable_v<get_debug_child_location_t, T &>)
                -> tag_invoke_result_t<get_debug_child_location_t, T &>
        {
            return tag_invoke(*this, t);
        }
    };

    inline constexpr get_debug_child_location_t get_debug_child_location;

    struct DebugLocation {
        std::vector<bool> mBreakpoints;
    };

    struct MADGINE_DEBUGGER_EXPORT ContextInfo final {

        bool wantsPause(TypedPtr location, ContinuationType type, IndexType<size_t> line);
        void suspend(TypedPtr location, ContinuationType type);

        bool alive() const;
        bool isPaused() const;

        mutable std::mutex mMutex;

        void setBreakpoint(const void *location, size_t index, bool set) const;
        bool getBreakpoint(const void *location, size_t index) const;

        void stepInto(TypedPtr &parent, TypedPtr location);
        void stepOut(TypedPtr &parent, TypedPtr location);

        TypedPtr mChild;

        friend TypedPtr &tag_invoke(get_debug_child_location_t, ContextInfo &info)
        {
            return info.mChild;
        }

    private:
        mutable std::map<const void *, DebugLocation> mDebugLocations;
    };

    inline constexpr auto with_debug_context = [](ContextInfo &info) { return pipable_from_right([](auto &&sender, ContextInfo &info) {
                                                                           return std::forward<decltype(sender)>(sender) | Execution::with_query_value(get_debug_context, info) | Execution::with_query_value(get_debug_child_location, info.mChild);
                                                                       },
                                                                           info); };

    void stepInto(auto &rec, TypedPtr location)
    {
        get_debug_context(rec).stepInto(get_debug_child_location(rec), location);
    }
    void stepOut(auto &rec, TypedPtr location)
    {
        get_debug_context(rec).stepOut(get_debug_child_location(rec), location);
    }

}
}