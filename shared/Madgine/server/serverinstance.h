#pragma once

#if ENABLE_THREADING

#    include "Modules/threading/workgrouphandle.h"

namespace Engine {
namespace Server {
    struct MADGINE_SERVER_EXPORT ServerInstance {

        ServerInstance() = delete;
        ServerInstance(const ServerInstance &) = delete;

        template <typename F, typename... Args>
        ServerInstance(std::string_view name, F &&initCallback, Args &&... args)
            : mName(name.empty() ? "thread_"s + std::to_string(++sInstanceCounter) : name)
            , mWorkGroup(mName, &ServerInstance::go<F, Args...>, this, std::forward<F>(initCallback), std::forward<Args>(args)...)
        {
        }

        const char *key() const;

        //ValueType toValueType() const;

    protected:
        template <typename F, typename... Args>
        int go(F&& initCallback, Args&&... args)
        {
            return TupleUnpacker::invokeDefaultResult(0, std::forward<F>(initCallback), std::forward<Args>(args)...);
        }

    private:
        std::string mName;
        static size_t sInstanceCounter;

        Threading::WorkGroupHandle mWorkGroup;
    };
}
}

#endif