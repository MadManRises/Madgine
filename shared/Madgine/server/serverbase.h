#pragma once

#if ENABLE_THREADING

#    include "serverlog.h"

#    include "Modules/threading/taskqueue.h"
#    include "serverinstance.h"

namespace Engine {
namespace Server {
    struct MADGINE_SERVER_EXPORT ServerBase {
        ServerBase(const std::string &name);
        virtual ~ServerBase();

        ServerLog &log();

        void shutdown();

    protected:
        virtual void performCommand(const std::string &cmd);

        template <typename F, typename... Args>
        void spawnNamedInstance(std::string_view name, F &&init, Args &&...args)
        {
            mInstances.emplace_back(name, std::forward<F>(init), std::forward<Args>(args)...);
        }

        template <typename F, typename... Args>
        void spawnInstance(F &&init, Args &&...args)
        {
            spawnNamedInstance({}, std::forward<F>(init), std::forward<Args>(args)...);
        }

        void consoleCheck();

        Threading::TaskQueue mTaskQueue;

    private:
        ServerLog mLog;

        std::list<ServerInstance> mInstances;
    };

}
}

REGISTER_TYPE(Engine::Server::ServerBase)

#endif