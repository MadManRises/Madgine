#pragma once

namespace Engine {
namespace Serialize {

    struct RequestPolicyCategory;

    template <bool callByMasterOnly>
    struct RequestPolicyType {
        using Category = RequestPolicyCategory;

        static constexpr bool sCallByMasterOnly = callByMasterOnly;
    };

    namespace RequestPolicy {
        using all_requests = RequestPolicyType<false>;
        using no_requests = RequestPolicyType<true>;
    };

    template <typename... Configs>
    using RequestPolicySelector = ConfigSelectorDefault<RequestPolicyCategory, RequestPolicy::all_requests, Configs...>;

}
}