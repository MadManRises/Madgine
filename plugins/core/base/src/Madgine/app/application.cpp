#include "../baselib.h"
#include "application.h"

#include "appsettings.h"

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
            [this]() { return callInit(); },
            [this]() { return callFinalize(); });
    }

    Application::~Application()
    {
        assert(sApp == this);
        sApp = nullptr;
    }

    Threading::Task<bool> Application::init()
    {
        for (const std::unique_ptr<GlobalAPIBase> &api : mGlobalAPIs) {
            if (!co_await api->callInit())
                co_return false;
        }

        co_return true;
    }

    Threading::Task<void> Application::finalize()
    {
        for (const std::unique_ptr<GlobalAPIBase> &api : mGlobalAPIs) {
            co_await api->callFinalize();
        }
    }

    GlobalAPIBase &Application::getGlobalAPIComponent(size_t i)
    {
        return mGlobalAPIs.get(i);
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
