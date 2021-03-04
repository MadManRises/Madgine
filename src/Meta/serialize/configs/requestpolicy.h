#pragma once

namespace Engine {
namespace Serialize {

    struct RequestPolicyCategory;

    template <bool callByMasterOnly>
    struct RequestPolicyType {
        using Category = RequestPolicyCategory;

        static constexpr bool sCallByMasterOnly = callByMasterOnly;
    };

    struct RequestPolicy {
        using all_requests = RequestPolicyType<false>;
        using no_requests = RequestPolicyType<true>;
    };

}
}