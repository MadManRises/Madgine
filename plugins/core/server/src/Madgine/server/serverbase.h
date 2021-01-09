#pragma once

#if ENABLE_THREADING

#    include "serverlog.h"

#    include "Modules/threading/taskqueue.h"
#    include "serverinstance.h"

namespace Engine {
namespace Server {
    struct MADGINE_SERVER_EXPORT ServerBase : public ScopeBase, public Threading::TaskQueue {    
        ServerBase(const std::string &name);
        virtual ~ServerBase();

        ServerLog &log();

        void shutdown();

    protected:
        virtual void performCommand(const std::string &cmd);

        template <typename T>
        void spawnInstance(T &&init)
        {
            mInstances.emplace_back(std::forward<T>(init));
        }

        void consoleCheck();

    private:
        ServerLog mLog;

        std::list<ServerInstance> mInstances;
    };

}
}

RegisterType(Engine::Server::ServerBase);

#endif