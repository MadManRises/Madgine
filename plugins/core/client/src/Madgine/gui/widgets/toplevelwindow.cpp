#include "../../clientlib.h"

#include "toplevelwindow.h"

#include "../../ui/uimanager.h"

#include "../../input/inputhandler.h"

#include "Modules/keyvalue/keyvalueiterate.h"

#include "Interfaces/window/windowapi.h"

#include "../../render/rendercontext.h"

#include "Modules/generic/transformIt.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/serializetable_impl.h"

#include "Modules/debug/profiler/profiler.h"

#include "toolwindow.h"

#include "Interfaces/exception.h"

#include "Modules/generic/reverseIt.h"

#include "../../render/rendertarget.h"

METATABLE_BEGIN(Engine::GUI::TopLevelWindow)
READONLY_PROPERTY(Components, components)
METATABLE_END(Engine::GUI::TopLevelWindow)

RegisterType(Engine::GUI::TopLevelWindow);

SERIALIZETABLE_BEGIN(Engine::GUI::TopLevelWindow)
FIELD(mComponents)
SERIALIZETABLE_END(Engine::GUI::TopLevelWindow)

namespace Engine {

namespace GUI {
    TopLevelWindow::TopLevelWindow(const Window::WindowSettings &settings)
        : mComponents(*this)
        , mSettings(settings)
    {

        mLoop.addSetupSteps(
            [this]() {
                if (!callInit())
                    throw exception("Window Init Failed!");
            },
            [this]() {
                callFinalize();
            });
    }

    TopLevelWindow::~TopLevelWindow()
    {
    }

    Rect2i TopLevelWindow::getScreenSpace()
    {
        if (!mWindow)
            return { { 0, 0 }, { 0, 0 } };
        return { { mWindow->renderX(), mWindow->renderY() }, { mWindow->renderWidth(), mWindow->renderHeight() } };
    }

    const MadgineObject *TopLevelWindow::parent() const
    {
        return nullptr;
    }

    bool TopLevelWindow::init()
    {

        mWindow = Window::sCreateWindow(mSettings);

        mWindow->addListener(this);

        if (mSettings.mInput) {
            mExternalInput = static_cast<Input::InputHandler *>(mSettings.mInput);
        } else {
            mInputHandlerSelector.emplace(*this, mWindow, this, 0);
        }

        mRenderContext.emplace();
        mRenderWindow = (*mRenderContext)->createRenderWindow(mWindow);
        addFrameListener(this);

        //WidgetBase *loading = createTopLevelImage("Loading");
        //WidgetBase *progress = loading->createChildBar("ProgressBar");
        //progress->setSize({ 0.8f, 0, 0, 0, 0.1f, 0, 0, 0, 1 });
        //progress->setPos({ 0.1f, 0, 0, 0, 0.85f, 0, 0, 0, 0 });
        //WidgetBase *loadMsg = loading->createChildLabel("LoadingMsg");
        //WidgetBase *ingame = createTopLevelWidget("ingame");
        //SceneWindow *game = ingame->createChildSceneWindow("game");
        ////game->setCamera(gui.sceneMgr().createCamera());
        //game->setSize({ 0.8f, 0, 0, 0, 1, 0, 0, 0, 1 });
        //WidgetBase *placeBaseButton = game->createChildButton("PlaceBaseButton");
        //placeBaseButton->setPos({ 1, 0, -120, 0, 0, 30, 0, 0, 0 });
        //placeBaseButton->setSize({ 0, 0, 100, 0, 0, 30, 0, 0, 1 });
        //WidgetBase *endTurnButton = game->createChildButton("EndTurnButton");
        //endTurnButton->setPos({ 1, 0, -120, 0, 0, 30, 0, 0, 0 });
        //endTurnButton->setSize({ 0, 0, 100, 0, 0, 30, 0, 0, 1 });
        //WidgetBase *unitUI = ingame->createChildWidget("UnitUI");
        //unitUI->setPos({ 0.8f, 0, 0, 0, 0, 0, 0, 0, 0 });
        //unitUI->setSize({ 0.2f, 0, 0, 0, 1, 0, 0, 0, 1 });
        //WidgetBase *fieldImage = unitUI->createChildImage("FieldImage");
        //fieldImage->setPos({ 0, 0, 0, 0, 0, 0, 0, 0, 0 });
        //fieldImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
        //WidgetBase *defenderImage = unitUI->createChildImage("DefenderImage");
        //defenderImage->setPos({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 0 });
        //defenderImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
        //WidgetBase *defenderHealth = defenderImage->createChildBar("DefenderHealth");
        //defenderHealth->setPos({ 0, 0, 0, 0, 0.85f, 0, 0, 0, 0 });
        //defenderHealth->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //WidgetBase *defenderExp = defenderImage->createChildBar("DefenderExp");
        //defenderExp->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        //defenderExp->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //WidgetBase *attackerImage = unitUI->createChildImage("AttackerImage");
        //attackerImage->setPos({ 0, 0, 0, 0.5f, 0, 0, 0, 0, 0 });
        //attackerImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
        //WidgetBase *attackerHealth = attackerImage->createChildBar("AttackerHealth");
        //attackerHealth->setPos({ 0, 0, 0, 0, 0.85f, 0, 0, 0, 0 });
        //attackerHealth->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //WidgetBase *attackerExp = attackerImage->createChildBar("AttackerExp");
        //attackerExp->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        //attackerExp->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //WidgetBase *modifierImage = unitUI->createChildImage("ModifierImage");
        //modifierImage->setPos({ 1.15f, 0, 0, 0.15f, 0, 0, 0, 0, 0 });
        //modifierImage->setSize({ 0.2f, 0, 0, 0.2f, 0, 0, 0, 0, 1 });
        //Image *attackerStrengthIcon = unitUI->createChildImage("AttackerStrengthIcon");
        //attackerStrengthIcon->setPos({ 0.025f, 0, 0, 1.025f, 0, 0, 0, 0, 0 });
        //attackerStrengthIcon->setSize({ 0.15f, 0, 0, 0.15f, 0, 0, 0, 0, 1 });
        //attackerStrengthIcon->setImageByName("Sword-01");
        //WidgetBase *attackerStrength = unitUI->createChildLabel("AttackerStrength");
        //attackerStrength->setPos({ 0.2f, 0, 0, 1.025f, 0, 0, 0, 0, 0 });
        //attackerStrength->setSize({ 0.2f, 0, 0, 0.15f, 0, 0, 0, 0, 1 });
        //WidgetBase *strengthArrow = unitUI->createChildImage("StrengthArrow");
        //strengthArrow->setPos({ 0.4f, 0, 0, 1.075f, 0, 0, 0, 0, 0 });
        //strengthArrow->setSize({ 0.2f, 0, 0, 0.05f, 0, 0, 0, 0, 1 });
        //WidgetBase *defenderStrength = unitUI->createChildLabel("DefenderStrength");
        //defenderStrength->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        //defenderStrength->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //WidgetBase *defenderStrengthIcon = unitUI->createChildImage("DefenderStrengthIcon");
        //defenderStrengthIcon->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        //defenderStrengthIcon->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //WidgetBase *contentWindow = unitUI->createChildWidget("ContentWindow");
        //contentWindow->setPos({ 0, 0, 0, 1, 0, 0, 0, 0, 0 });
        //contentWindow->setSize({ 1, 0, 0, -1, 1, 0, 0, 0, 1 });
        //WidgetBase *movementIcon = unitUI->createChildImage("MovementIcon");
        //movementIcon->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        //movementIcon->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //WidgetBase *movement = unitUI->createChildLabel("Movement");
        //movement->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        //movement->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //WidgetBase *health = unitUI->createChildLabel("Health");
        //health->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        //health->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //WidgetBase *mainMenu = createTopLevelWidget("MainMenu");
        //WidgetBase *optionsButton = mainMenu->createChildButton("OptionsButton");
        //optionsButton->setSize({ 0.4f, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //optionsButton->setPos({ 0.55f, 0, 0, 0, 0.55f, 0, 0, 0, 0 });
        //WidgetBase *findMatchButton = mainMenu->createChildButton("FindMatchButton");
        //findMatchButton->setSize({ 0.4f, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //findMatchButton->setPos({ 0.55f, 0, 0, 0, 0.8f, 0, 0, 0, 0 });
        //WidgetBase *playButton = mainMenu->createChildButton("PlayButton");
        //playButton->setSize({ 0.4f, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //playButton->setPos({ 0.05f, 0, 0, 0, 0.55f, 0, 0, 0, 0 });
        //WidgetBase *quitButton = mainMenu->createChildButton("QuitButton");
        //quitButton->setSize({ 0.4f, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        //quitButton->setPos({ 0.05f, 0, 0, 0, 0.8f, 0, 0, 0, 0 });
        //Image *titleScreen = mainMenu->createChildImage("noname");
        //titleScreen->setImageByName("TitleScreen");
        //titleScreen->setSize({ 0.8f, 0, 0, 0.1f, 0, 0, 0, 0, 1 });
        //titleScreen->setPos({ 0.1f, 0, 0, 0, 0.05f, 0, 0, 0, 0 });
        //WidgetBase *connectionFailureWindow = createTopLevelWidget("ConnectionFailureWindow");
        //WidgetBase *connectionLabel = connectionFailureWindow->createChildLabel("ConnectionLabel");
        //WidgetBase *retryButton = connectionFailureWindow->createChildButton("RetryButton");
        //WidgetBase *abortButton = connectionFailureWindow->createChildButton("AbortButton");
        //WidgetBase *lobbyMenu = createTopLevelWidget("LobbyMenu");
        //WidgetBase *startGameButton = lobbyMenu->createChildButton("StartGameButton");
        //WidgetBase *lobbyListMenu = createTopLevelWidget("LobbyListMenu");
        //WidgetBase *createLobbyButton = lobbyListMenu->createChildButton("CreateLobbyButton");
        //WidgetBase *joinLobbyButton = lobbyListMenu->createChildButton("JoinLobbyButton");
        //WidgetBase *backButton = lobbyListMenu->createChildButton("BackButton");
        //WidgetBase *lobbyList = lobbyListMenu->createChildCombobox("LobbyList");

        for (TopLevelWindowComponentBase *comp : uniquePtrToPtr(mComponents)) {
            bool result = comp->callInit();
            assert(result);
        }

		applyClientSpaceResize();

        return true;
    }

    void TopLevelWindow::finalize()
    {
        for (TopLevelWindowComponentBase *comp : uniquePtrToPtr(mComponents)) {
            comp->callFinalize();
        }

        removeFrameListener(this);

        mRenderWindow.reset();
        mRenderContext.reset();

        mInputHandlerSelector.reset();

        if (mWindow) {
            mWindow->removeListener(this);
            mWindow->destroy();
        }
    }

    ToolWindow *TopLevelWindow::createToolWindow(const Window::WindowSettings &settings)
    {
        return mToolWindows.emplace_back(std::make_unique<ToolWindow>(*this, settings)).get();
    }

    void TopLevelWindow::destroyToolWindow(ToolWindow *w)
    {
        auto it = std::find_if(mToolWindows.begin(), mToolWindows.end(), [=](const std::unique_ptr<ToolWindow> &ptr) { return ptr.get() == w; });
        assert(it != mToolWindows.end());
        mToolWindows.erase(it);
    }

    Input::InputHandler *TopLevelWindow::input()
    {
        return mExternalInput ? mExternalInput : *mInputHandlerSelector;
    }

    bool TopLevelWindow::injectKeyPress(const Input::KeyEventArgs &arg)
    {
        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectKeyPress(arg))
                return true;
        }
        return false;
    }

    bool TopLevelWindow::injectKeyRelease(const Input::KeyEventArgs &arg)
    {
        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectKeyRelease(arg))
                return true;
        }
        return false;
    }

    bool TopLevelWindow::injectPointerPress(const Input::PointerEventArgs &arg)
    {

        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectPointerPress(arg))
                return true;
        }

        return false;
    }

    bool TopLevelWindow::injectPointerRelease(const Input::PointerEventArgs &arg)
    {

        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectPointerRelease(arg))
                return true;
        }

        return false;
    }

    bool TopLevelWindow::injectPointerMove(const Input::PointerEventArgs &arg)
    {

        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
            if (comp->injectPointerMove(arg))
                return true;
        }

        return false;
    }

    Window::Window *TopLevelWindow::window() const
    {
        return mWindow;
    }

    Render::RenderContext *TopLevelWindow::getRenderer()
    {
        return mRenderContext->get();
    }

    void TopLevelWindow::onClose()
    {
        mWindow = nullptr;
        callFinalize();
    }

    void TopLevelWindow::onRepaint()
    {
        //update();
    }

    void TopLevelWindow::onResize(size_t width, size_t height)
    {
        input()->onResize(width, height);
        applyClientSpaceResize();
    }

    void TopLevelWindow::applyClientSpaceResize(TopLevelWindowComponentBase *component)
    {
        Rect2i space;
        if (!component)
			space = { { 0, 0 }, { mWindow->renderWidth(), mWindow->renderHeight() } };
        else
            space = component->getChildClientSpace();

        for (TopLevelWindowComponentBase *comp : reverseIt(components())) {
            if (component) {
                if (component == comp) {
                    component = nullptr;                    
                }
            } else {
                comp->mClientSpace = space;
                space = comp->getChildClientSpace();
            }
        }
    }

    bool TopLevelWindow::frameStarted(std::chrono::microseconds)
    {
        return true;
    }

    bool TopLevelWindow::frameRenderingQueued(std::chrono::microseconds, Threading::ContextMask)
    {

        return mWindow;
    }

    bool TopLevelWindow::frameEnded(std::chrono::microseconds)
    {
        (*mRenderContext)->render();
        Window::sUpdate();
        return mWindow;
    }

    TopLevelWindowComponentBase &TopLevelWindow::getWindowComponent(size_t i, bool init)
    {
        TopLevelWindowComponentBase &component = mComponents.get(i);
        if (init) {
            component.callInit();
        }
        return component.getSelf(init);
    }

    void TopLevelWindow::addFrameListener(Threading::FrameListener *listener)
    {
        mLoop.addFrameListener(listener);
    }

    void TopLevelWindow::removeFrameListener(Threading::FrameListener *listener)
    {
        mLoop.removeFrameListener(listener);
    }

    void TopLevelWindow::singleFrame()
    {
        mLoop.singleFrame();
    }

    Threading::FrameLoop &TopLevelWindow::frameLoop()
    {
        return mLoop;
    }

    void TopLevelWindow::shutdown()
    {
        mLoop.shutdown();
    }

    Render::RenderTarget *TopLevelWindow::getRenderWindow()
    {
        return mRenderWindow.get();
    }

}
}
