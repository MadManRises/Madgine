#include "../serverlib.h"

#if ENABLE_THREADING

#include "serverbase.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/reflection/classname.h"

namespace Engine {
namespace Server {
    ServerBase::ServerBase(Threading::WorkGroup &workgroup)
        : TaskQueue("Default")
        , mLog(workgroup.name() + "-Log")
    {
        Util::setLog(&mLog);
        mLog.startConsole();
        addRepeatedTask([this]() { consoleCheck(); }, std::chrono::milliseconds(20));
    }

    ServerBase::~ServerBase()
    {
        removeRepeatedTasks(this);
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
        stop();
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
        mLastConsoleCheck = std::chrono::steady_clock::now();
    }

}
}

METATABLE_BEGIN(Engine::Server::ServerBase)
METATABLE_END(Engine::Server::ServerBase)

RegisterType(Engine::Server::ServerBase);

#endif