#pragma once

#include "configselector.h"

namespace Engine {
namespace Serialize {

    struct VerifierCategory;

    struct DefaultVerifier {
        using Category = VerifierCategory;

        static bool verify(const CallerHierarchyBasePtr &hierarchy, ParticipantId requester)
        {
            return true;
        }
    };

    template <auto f, typename R, typename T, typename... Args>
    struct CustomVerifierImpl {
        using Category = VerifierCategory;

        static bool verify(const CallerHierarchyBasePtr &hierarchy, Args... args)
        {
            return f(args...);
        }
    };

    template <auto f, typename R, typename T, typename... Args>
    struct ParentVerifierImpl {
        using Category = VerifierCategory;

        static bool verify(const CallerHierarchyBasePtr &hierarchy, Args... args)
        {
            T *parent = static_cast<T *>(hierarchy);
            return (parent->*f)(args...);
        }
    };

    template <auto f>
    using ParentVerifier = typename FunctionCapture<ParentVerifierImpl, f>::type;
    template <auto f>
    using CustomVerifier = typename FunctionCapture<CustomVerifierImpl, f>::type;

    template <typename... Configs>
    using VerifierSelector = ConfigSelectorDefault<VerifierCategory, DefaultVerifier, Configs...>;

}
}