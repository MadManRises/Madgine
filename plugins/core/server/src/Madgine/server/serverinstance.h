#pragma once

#if ENABLE_THREADING

#    include "Modules/threading/workgrouphandle.h"

namespace Engine {
namespace Server {
    struct MADGINE_SERVER_EXPORT ServerInstance {

        ServerInstance() = delete;
        ServerInstance(const ServerInstance &) = delete;

        template <typename T>
        ServerInstance(T &&initCallback)
            : mName("thread_"s + std::to_string(++sInstanceCounter))
            , mWorkGroup(&ServerInstance::go<T>, this, std::forward<T>(initCallback))
        {
        }

        const char *key() const;

        //ValueType toValueType() const;

    protected:
        template <typename T>
        int go(T initCallback, Threading::WorkGroup &workgroup)
        {
            return TupleUnpacker::invokeDefaultResult(0, std::move(initCallback), workgroup, &mHandle);
        }

    private:
        ScopeBase *mHandle = nullptr;

        std::string mName;
        static size_t sInstanceCounter;

        Threading::WorkGroupHandle mWorkGroup;
    };
}
}

#endif