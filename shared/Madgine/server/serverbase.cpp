#include "serverlib.h"

#if ENABLE_THREADING

#    include "serverbase.h"

#    include "Meta/keyvalue/metatable_impl.h"

#include "Modules/threading/awaitables/awaitabletimepoint.h"

METATABLE_BEGIN(Engine::Server::ServerBase)
METATABLE_END(Engine::Server::ServerBase)

namespace Engine {
namespace Server {
    ServerBase::ServerBase(const std::string &name)
        : mTaskQueue(name + "-Server")
        , mLog(name + "-Log")
    {
        Log::setLog(&mLog);
        mLog.startConsole();
        mTaskQueue.queue([this]() -> Threading::Task<void> {
            while (mTaskQueue.running()) {
                consoleCheck();
                co_await 20ms;
            }
        });
    }

    ServerBase::~ServerBase()
    {
        mLog.stopConsole();
        mInstances.clear();
        Log::setLog(nullptr);
    }

    ServerLog &ServerBase::log()
    {
        return mLog;
    }

    void ServerBase::shutdown()
    {
        mTaskQueue.stop();
    }

    void ServerBase::performCommand(const std::string &cmd)
    {
        if (cmd == "shutdown") {
            shutdown();
        } else {
            LOG("Unknown Command \"" << cmd << "\"!");
        }
    }

    void ServerBase::consoleCheck()
    {
        for (const std::string &cmd : mLog.update()) {
            performCommand(cmd);
        }
    }

}
}

#endif