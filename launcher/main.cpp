#include "Madgine/baselib.h"
#include "Madgine/resourceslib.h"

#include "Interfaces/filesystem/api.h"

#include "Madgine/cli/parameter.h"

#include "Modules/threading/workgroup.h"

#include "Madgine/base/root.h"

#include "Interfaces/util/standardlog.h"

#include "launcher.h"

#include "main.h"

#include "Madgine/base/keyvalueregistry.h"

#include "Madgine/resources/resourcemanager.h"

Engine::CLI::Parameter<bool> toolMode { { "--toolMode", "-t" }, false, "If set, no application will be started. Only the root will be initialized and then immediately shutdown again." };

Engine::CLI::Parameter<Engine::Util::MessageType> logLevel { { "--logLevel", "-l" }, Engine::Util::MessageType::INFO_TYPE, "Specify log-level." };


int desktopMain(int argc, char **argv)
{
    Engine::Filesystem::setup();
    Engine::Threading::WorkGroup workGroup { "Launcher" };
    Engine::Base::Root root { argc, argv };
    Engine::Util::StandardLog::setLogLevel(logLevel);

    Engine::KeyValueGlobal<Engine::Resources::ResourceManager> resourceManager { "ResourceManager", toolMode };

    if (!toolMode) {
        return launch();
    } else {
        return root.errorCode();
    }
}

#if !EMSCRIPTEN && !OSX && !IOS
DLL_EXPORT_TAG int main(int argc, char **argv)
{
    return desktopMain(argc, argv);
}
#endif