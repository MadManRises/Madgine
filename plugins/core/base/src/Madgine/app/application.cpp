#include "../baselib.h"
#include "application.h"

#include "appsettings.h"

#include "Interfaces/util/exception.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Modules/threading/workgroupstorage.h"

#include "globalapibase.h"


METATABLE_BEGIN(Engine::App::Application)
MEMBER(mGlobalAPIs)
METATABLE_END(Engine::App::Application)


namespace Engine {

namespace App {

    static Threading::WorkgroupLocal<Application *> sApp;

    Application::Application(const AppSettings &settings)
        : mSettings(settings)
        , mTaskQueue("Application")
        , mGlobalAPIs(*this)
    {
        assert(!sApp);
        sApp = this;

        mTaskQueue.addSetupSteps(
            [this]() {
                if (!callInit())
                    throw exception("App Init Failed!");
                return true;
            },
            [this]() {
                callFinalize();
            });
    }

    Application::~Application()
    {
        assert(sApp == this);
        sApp = nullptr;
    }

    bool Application::init()
    {

        markInitialized();

        for (const std::unique_ptr<GlobalAPIBase> &api : mGlobalAPIs) {
            if (!api->callInitOrder(mGlobalAPIInitCounter))
                return false;
        }

        return true;
    }

    void Application::finalize()
    {
        for (; mGlobalAPIInitCounter > 0; --mGlobalAPIInitCounter) {
            for (const std::unique_ptr<GlobalAPIBase> &api : mGlobalAPIs) {
                api->callFinalizeOrder(mGlobalAPIInitCounter);
            }
        }
    }

    GlobalAPIBase &Application::getGlobalAPIComponent(size_t i, bool init)
    {
        return getChildOrder(mGlobalAPIs.get(i), mGlobalAPIInitCounter, init);
    }

    const AppSettings &Application::settings()
    {
        return mSettings;
    }

    Application &Application::getSingleton()
    {
        return *sApp;
    }

    Threading::TaskQueue *Application::taskQueue()
    {
        return &mTaskQueue;
    }
}

}
