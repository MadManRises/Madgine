#pragma once

namespace Engine {
namespace Serialize {

struct GuardCategory;

template <typename... Guards>
struct Guard {
    using Category = GuardCategory;
    
    static std::tuple<> begin()
    {
        return {};
    }

    static void end(std::tuple<>)
    {
    
    }
};

}
}