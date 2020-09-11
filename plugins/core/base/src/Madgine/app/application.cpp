#include "../baselib.h"
#include "application.h"

#include "appsettings.h"

#include "Interfaces/util/exception.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/threading/workgroupstorage.h"

#include "globalapibase.h"

#include "Modules/threading/defaulttaskqueue.h"

#include "Modules/plugins/pluginmanager.h"
#include "Modules/plugins/pluginsection.h"

namespace Engine {

namespace App {

    static Threading::WorkgroupLocal<Application *> sApp;

    Application::Application(const AppSettings &settings)
        : mSettings(settings)
        , mGlobalAPIInitCounter(0)
        , mTaskQueue("Application")
        , mGlobalAPIs(*this)
    {
        assert(!sApp);
        sApp = this;

        mTaskQueue.addSetupSteps(
            [this]() {
                if (!callInit())
                    throw exception("App Init Failed!");
                //return false;
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
            if (!api->callInit(mGlobalAPIInitCounter))
                return false;
        }

        return true;
    }

    void Application::finalize()
    {
        for (; mGlobalAPIInitCounter > 0; --mGlobalAPIInitCounter) {
            for (const std::unique_ptr<GlobalAPIBase> &api : mGlobalAPIs) {
                api->callFinalize(mGlobalAPIInitCounter);
            }
        }
    }

    GlobalAPIBase &Application::getGlobalAPIComponent(size_t i, bool init)
    {
        GlobalAPIBase &api = mGlobalAPIs.get(i);
        if (init) {
            checkInitState();
            api.callInit(mGlobalAPIInitCounter);
        }
        return api.getSelf(init);
    }

    Application &Application::getSelf(bool init)
    {
        if (init) {
            checkDependency();
        }
        return *this;
    }

    const AppSettings &Application::settings()
    {
        return mSettings;
    }

    const MadgineObject *Application::parent() const
    {
        return nullptr;
    }

    Application &Application::getSingleton()
    {
        return *sApp;
    }
}

}

METATABLE_BEGIN(Engine::App::Application)
MEMBER(mGlobalAPIs)
METATABLE_END(Engine::App::Application)
