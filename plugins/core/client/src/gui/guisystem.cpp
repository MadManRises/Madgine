#include "../clientlib.h"

#include "Madgine/app/application.h"
#include "guisystem.h"

#include "widgets/toplevelwindow.h"

#include "widgets/widget.h"

#include "Modules/keyvalue/keyvalueiterate.h"

#include "Interfaces/window/windowapi.h"

#include "Modules/debug/profiler/profiler.h"

#include "Modules/reflection/classname.h"
#include "Modules/keyvalue/metatable_impl.h"

UNIQUECOMPONENT(Engine::GUI::GUISystem);

namespace Engine {

namespace GUI {

    GUISystem::GUISystem(App::Application &app)
        : App::GlobalAPI<GUISystem>(app)
        , mRenderer(this)
    {
    }

    GUISystem::~GUISystem()
    {
    }

    bool GUISystem::init()
    {
        app(false).addFrameListener(this);

        markInitialized();

        if (!mRenderer->callInit())
            return false;

        createTopLevelWindow();

        return true;
    }

    void GUISystem::finalize()
    {

        clear();

        mRenderer->callFinalize();

        app(false).removeFrameListener(this);
    }

    void GUISystem::clear()
    {
        mWindows.clear();
    }

    /*KeyValueMapList GUISystem::maps()
		{
			return Scope::maps().merge(mWindows, mRenderer.get());
		}*/

    std::vector<std::unique_ptr<TopLevelWindow>> &GUISystem::topLevelWindows()
    {
        return mWindows;
    }

    void GUISystem::closeTopLevelWindow(TopLevelWindow *w)
    {        
        mWindows.erase(std::find_if(mWindows.begin(), mWindows.end(), [w](const std::unique_ptr<TopLevelWindow> &p) { return p.get() == w; }));
    }

    Render::RendererBase &GUISystem::renderer()
    {
        return *mRenderer;
    }

    TopLevelWindow *GUISystem::createTopLevelWindow()
    {
        TopLevelWindow *w = mWindows.emplace_back(std::make_unique<TopLevelWindow>(*this)).get();        
        return w;
    }

    bool GUISystem::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
    {
        PROFILE();
        Window::sUpdate();
        return !mWindows.empty();
    }

    GUISystem &GUISystem::getSelf(bool init)
    {
        if (init) {
            checkDependency();
        }
        return *this;
    }

    Scene::SceneComponentBase &GUISystem::getSceneComponent(size_t i, bool init)
    {
        if (init) {
            checkInitState();
        }
        return app(false).getSceneComponent(i, init);
    }

    App::GlobalAPIBase &GUISystem::getGlobalAPIComponent(size_t i, bool init)
    {
        if (init) {
            checkInitState();
        }
        return app(false).getGlobalAPIComponent(i, init);
    }

    Scene::SceneManager &GUISystem::sceneMgr(bool init)
    {
        if (init) {
            checkInitState();
        }
        return app(false).sceneMgr(init);
    }

    const std::vector<std::unique_ptr<TopLevelWindow>> &GUISystem::windows()
    {
        return mWindows;
    }

}
}

METATABLE_BEGIN(Engine::GUI::GUISystem)
READONLY_PROPERTY(Windows, windows)
METATABLE_END(Engine::GUI::GUISystem)

RegisterType(Engine::GUI::GUISystem);
