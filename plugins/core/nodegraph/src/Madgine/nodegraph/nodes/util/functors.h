#pragma once

namespace Engine {
namespace NodeGraph {

	
    struct Add {
        auto operator()(const auto &...v) const { return (v + ...); };
    };

    struct Log {
        void operator()(const auto &...v) const
        {
            (Engine::Util::LogDummy { Engine::Util::MessageType::INFO_TYPE } << ... << v);
        }
    };


}
}