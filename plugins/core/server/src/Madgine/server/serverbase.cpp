#include "../serverlib.h"

#if ENABLE_THREADING

#    include "serverbase.h"

#    include "Meta/keyvalue/metatable_impl.h"

METATABLE_BEGIN(Engine::Server::ServerBase)
METATABLE_END(Engine::Server::ServerBase)

namespace Engine {
namespace Server {
    ServerBase::ServerBase(const std::string &name)
        : mTaskQueue(name + "-Server")
        , mLog(name + "-Log")
    {
        //Util::setLog(&mLog);
        mLog.startConsole();
        mTaskQueue.addRepeatedTask([this]() { consoleCheck(); }, std::chrono::milliseconds(20));
    }

    ServerBase::~ServerBase()
    {
        mTaskQueue.removeRepeatedTasks(this);
        mLog.stopConsole();
        mInstances.clear();
        Util::setLog(nullptr);
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