#include "../../clientlib.h"

#include "toplevelwindow.h"

#include "../../ui/uimanager.h"

#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "image.h"
#include "label.h"
#include "scenewindow.h"
#include "tabwidget.h"
#include "textbox.h"

#include "../guisystem.h"

#include "../../input/inputhandler.h"

#include "Modules/keyvalue/keyvalueiterate.h"

#include "Interfaces/window/windowapi.h"

#include "Madgine/app/application.h"
#include "Madgine/app/appsettings.h"

#include "../windowoverlay.h"

#include "../../render/renderwindow.h"

#include "Modules/generic/transformIt.h"

#include "Madgine/scene/scenemanager.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/debug/profiler/profiler.h"

#include "toolwindow.h"

#include "Modules/serialize/serializetable_impl.h"

SERIALIZETABLE_BEGIN(Engine::GUI::TopLevelWindow)
FIELD(mTopLevelWidgets, Serialize::ParentCreator<&TopLevelWindow::createWidgetClassTuple>)
FIELD(mComponents)
SERIALIZETABLE_END(Engine::GUI::TopLevelWindow)

namespace Engine {

namespace GUI {
    TopLevelWindow::TopLevelWindow(GUISystem &gui)
        : mGui(gui)
        , mComponents(*this)
    {
        const App::AppSettings &settings = App::Application::getSingleton().settings();

        mWindow = Window::sCreateWindow(settings.mWindowSettings);

        mWindow->addListener(this);

        if (settings.mInput) {
            mExternalInput = static_cast<Input::InputHandler *>(settings.mInput);
        } else {
            mInputHandlerSelector.emplace(mWindow, App::Application::getSingleton(), this, 0);
        }

        mRenderWindow = gui.renderer().createWindow(mWindow, this);
        App::Application::getSingleton().addFrameListener(this);

        WidgetBase *loading = createTopLevelImage("Loading");
        WidgetBase *progress = loading->createChildBar("ProgressBar");
        progress->setSize({ 0.8f, 0, 0, 0, 0.1f, 0, 0, 0, 1 });
        progress->setPos({ 0.1f, 0, 0, 0, 0.85f, 0, 0, 0, 0 });
        WidgetBase *loadMsg = loading->createChildLabel("LoadingMsg");
        WidgetBase *ingame = createTopLevelWidget("ingame");
        SceneWindow *game = ingame->createChildSceneWindow("game");
        //game->setCamera(gui.sceneMgr().createCamera());
        game->setSize({ 0.8f, 0, 0, 0, 1, 0, 0, 0, 1 });
        WidgetBase *placeBaseButton = game->createChildButton("PlaceBaseButton");
        placeBaseButton->setPos({ 1, 0, -120, 0, 0, 30, 0, 0, 0 });
        placeBaseButton->setSize({ 0, 0, 100, 0, 0, 30, 0, 0, 1 });
        WidgetBase *endTurnButton = game->createChildButton("EndTurnButton");
        endTurnButton->setPos({ 1, 0, -120, 0, 0, 30, 0, 0, 0 });
        endTurnButton->setSize({ 0, 0, 100, 0, 0, 30, 0, 0, 1 });
        WidgetBase *unitUI = ingame->createChildWidget("UnitUI");
        unitUI->setPos({ 0.8f, 0, 0, 0, 0, 0, 0, 0, 0 });
        unitUI->setSize({ 0.2f, 0, 0, 0, 1, 0, 0, 0, 1 });
        WidgetBase *fieldImage = unitUI->createChildImage("FieldImage");
        fieldImage->setPos({ 0, 0, 0, 0, 0, 0, 0, 0, 0 });
        fieldImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
        WidgetBase *defenderImage = unitUI->createChildImage("DefenderImage");
        defenderImage->setPos({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 0 });
        defenderImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
        WidgetBase *defenderHealth = defenderImage->createChildBar("DefenderHealth");
        defenderHealth->setPos({ 0, 0, 0, 0, 0.85f, 0, 0, 0, 0 });
        defenderHealth->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        WidgetBase *defenderExp = defenderImage->createChildBar("DefenderExp");
        defenderExp->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        defenderExp->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        WidgetBase *attackerImage = unitUI->createChildImage("AttackerImage");
        attackerImage->setPos({ 0, 0, 0, 0.5f, 0, 0, 0, 0, 0 });
        attackerImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
        WidgetBase *attackerHealth = attackerImage->createChildBar("AttackerHealth");
        attackerHealth->setPos({ 0, 0, 0, 0, 0.85f, 0, 0, 0, 0 });
        attackerHealth->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        WidgetBase *attackerExp = attackerImage->createChildBar("AttackerExp");
        attackerExp->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        attackerExp->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        WidgetBase *modifierImage = unitUI->createChildImage("ModifierImage");
        modifierImage->setPos({ 1.15f, 0, 0, 0.15f, 0, 0, 0, 0, 0 });
        modifierImage->setSize({ 0.2f, 0, 0, 0.2f, 0, 0, 0, 0, 1 });
        Image *attackerStrengthIcon = unitUI->createChildImage("AttackerStrengthIcon");
        attackerStrengthIcon->setPos({ 0.025f, 0, 0, 1.025f, 0, 0, 0, 0, 0 });
        attackerStrengthIcon->setSize({ 0.15f, 0, 0, 0.15f, 0, 0, 0, 0, 1 });
        attackerStrengthIcon->setImageByName("Sword-01");
        WidgetBase *attackerStrength = unitUI->createChildLabel("AttackerStrength");
        attackerStrength->setPos({ 0.2f, 0, 0, 1.025f, 0, 0, 0, 0, 0 });
        attackerStrength->setSize({ 0.2f, 0, 0, 0.15f, 0, 0, 0, 0, 1 });
        WidgetBase *strengthArrow = unitUI->createChildImage("StrengthArrow");
        strengthArrow->setPos({ 0.4f, 0, 0, 1.075f, 0, 0, 0, 0, 0 });
        strengthArrow->setSize({ 0.2f, 0, 0, 0.05f, 0, 0, 0, 0, 1 });
        WidgetBase *defenderStrength = unitUI->createChildLabel("DefenderStrength");
        defenderStrength->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        defenderStrength->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        WidgetBase *defenderStrengthIcon = unitUI->createChildImage("DefenderStrengthIcon");
        defenderStrengthIcon->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        defenderStrengthIcon->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        WidgetBase *contentWindow = unitUI->createChildWidget("ContentWindow");
        contentWindow->setPos({ 0, 0, 0, 1, 0, 0, 0, 0, 0 });
        contentWindow->setSize({ 1, 0, 0, -1, 1, 0, 0, 0, 1 });
        WidgetBase *movementIcon = unitUI->createChildImage("MovementIcon");
        movementIcon->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        movementIcon->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        WidgetBase *movement = unitUI->createChildLabel("Movement");
        movement->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        movement->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        WidgetBase *health = unitUI->createChildLabel("Health");
        health->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
        health->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        WidgetBase *mainMenu = createTopLevelWidget("MainMenu");
        WidgetBase *optionsButton = mainMenu->createChildButton("OptionsButton");
        optionsButton->setSize({ 0.4f, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        optionsButton->setPos({ 0.55f, 0, 0, 0, 0.55f, 0, 0, 0, 0 });
        WidgetBase *findMatchButton = mainMenu->createChildButton("FindMatchButton");
        findMatchButton->setSize({ 0.4f, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        findMatchButton->setPos({ 0.55f, 0, 0, 0, 0.8f, 0, 0, 0, 0 });
        WidgetBase *playButton = mainMenu->createChildButton("PlayButton");
        playButton->setSize({ 0.4f, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        playButton->setPos({ 0.05f, 0, 0, 0, 0.55f, 0, 0, 0, 0 });
        WidgetBase *quitButton = mainMenu->createChildButton("QuitButton");
        quitButton->setSize({ 0.4f, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
        quitButton->setPos({ 0.05f, 0, 0, 0, 0.8f, 0, 0, 0, 0 });
        Image *titleScreen = mainMenu->createChildImage("noname");
        titleScreen->setImageByName("TitleScreen");
        titleScreen->setSize({ 0.8f, 0, 0, 0.1f, 0, 0, 0, 0, 1 });
        titleScreen->setPos({ 0.1f, 0, 0, 0, 0.05f, 0, 0, 0, 0 });
        WidgetBase *connectionFailureWindow = createTopLevelWidget("ConnectionFailureWindow");
        WidgetBase *connectionLabel = connectionFailureWindow->createChildLabel("ConnectionLabel");
        WidgetBase *retryButton = connectionFailureWindow->createChildButton("RetryButton");
        WidgetBase *abortButton = connectionFailureWindow->createChildButton("AbortButton");
        WidgetBase *lobbyMenu = createTopLevelWidget("LobbyMenu");
        WidgetBase *startGameButton = lobbyMenu->createChildButton("StartGameButton");
        WidgetBase *lobbyListMenu = createTopLevelWidget("LobbyListMenu");
        WidgetBase *createLobbyButton = lobbyListMenu->createChildButton("CreateLobbyButton");
        WidgetBase *joinLobbyButton = lobbyListMenu->createChildButton("JoinLobbyButton");
        WidgetBase *backButton = lobbyListMenu->createChildButton("BackButton");
        WidgetBase *lobbyList = lobbyListMenu->createChildCombobox("LobbyList");
		
        for (TopLevelWindowComponentBase *comp : uniquePtrToPtr(mComponents)) {
            bool result = comp->callInit();
            assert(result);
        }
    }

    TopLevelWindow::~TopLevelWindow()
    {
        for (TopLevelWindowComponentBase *comp : uniquePtrToPtr(mComponents)) {
            comp->callFinalize();
        }

        App::Application::getSingleton().removeFrameListener(this);
        mWindow->removeListener(this);

        mTopLevelWidgets.clear();

        mWindow->destroy();

        assert(mWidgets.empty());
    }

    void TopLevelWindow::close()
    {
        mGui.closeTopLevelWindow(this);
    }

    /*void TopLevelWindow::showCursor()
		{
			setCursorVisibility(true);
		}

		void TopLevelWindow::hideCursor()
		{
			setCursorVisibility(false);
		}*/

    Vector3 TopLevelWindow::getScreenSize()
    {
        return { (float)mWindow->renderWidth(), (float)mWindow->renderHeight(), 1.0f };
    }

    std::pair<Vector3, Vector3> TopLevelWindow::getAvailableScreenSpace()
    {
        Vector3 size = getScreenSize();
        Vector3 pos = Vector3::ZERO;
        for (WindowOverlay *overlay : mOverlays)
            overlay->calculateAvailableScreenSpace(mWindow, pos, size);
        return std::make_pair(pos, size);
    }

    WidgetBase *TopLevelWindow::createTopLevelWidget(const std::string &name)
    {
        std::unique_ptr<WidgetBase> w = createWidget(name);
        return mTopLevelWidgets.emplace_back(std::move(w)).get();
    }

    Bar *TopLevelWindow::createTopLevelBar(const std::string &name)
    {
        std::unique_ptr<Bar> p = createBar(name);
        Bar *b = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return b;
    }

    Button *TopLevelWindow::createTopLevelButton(const std::string &name)
    {
        std::unique_ptr<Button> p = createButton(name);
        Button *b = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return b;
    }

    Checkbox *TopLevelWindow::createTopLevelCheckbox(const std::string &name)
    {
        std::unique_ptr<Checkbox> p = createCheckbox(name);
        Checkbox *c = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return c;
    }

    Combobox *TopLevelWindow::createTopLevelCombobox(const std::string &name)
    {
        std::unique_ptr<Combobox> p = createCombobox(name);
        Combobox *c = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return c;
    }

    Image *TopLevelWindow::createTopLevelImage(const std::string &name)
    {
        std::unique_ptr<Image> p = createImage(name);
        Image *i = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return i;
    }

    Label *TopLevelWindow::createTopLevelLabel(const std::string &name)
    {
        std::unique_ptr<Label> p = createLabel(name);
        Label *l = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return l;
    }

    SceneWindow *TopLevelWindow::createTopLevelSceneWindow(const std::string &name)
    {
        std::unique_ptr<SceneWindow> p = createSceneWindow(name);
        SceneWindow *s = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return s;
    }

    TabWidget *TopLevelWindow::createTopLevelTabWidget(const std::string &name)
    {
        std::unique_ptr<TabWidget> p = createTabWidget(name);
        TabWidget *t = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return t;
    }

    Textbox *TopLevelWindow::createTopLevelTextbox(const std::string &name)
    {
        std::unique_ptr<Textbox> p = createTextbox(name);
        Textbox *t = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return t;
    }

    std::unique_ptr<WidgetBase> TopLevelWindow::createWidgetClass(const std::string &name, WidgetClass _class)
    {
        switch (_class) {
        case WidgetClass::WIDGET_CLASS:
            return createWidget(name);
        case WidgetClass::BAR_CLASS:
            return createBar(name);
        case WidgetClass::CHECKBOX_CLASS:
            return createCheckbox(name);
        case WidgetClass::LABEL_CLASS:
            return createLabel(name);
        case WidgetClass::TABWIDGET_CLASS:
            return createTabWidget(name);
        case WidgetClass::BUTTON_CLASS:
            return createButton(name);
        case WidgetClass::COMBOBOX_CLASS:
            return createCombobox(name);
        case WidgetClass::TEXTBOX_CLASS:
            return createTextbox(name);
        case WidgetClass::SCENEWINDOW_CLASS:
            return createSceneWindow(name);
        case WidgetClass::IMAGE_CLASS:
            return createImage(name);
        default:
            throw 0;
        }
    }

    std::tuple<std::unique_ptr<WidgetBase>> Engine::GUI::TopLevelWindow::createWidgetClassTuple(const std::string &name, WidgetClass _class)
    {
        return { createWidgetClass(name, _class) };
    }

    std::unique_ptr<WidgetBase> TopLevelWindow::createWidget(const std::string &name)
    {
        std::unique_ptr<WidgetBase> w = std::make_unique<WidgetBase>(name, *this);
        w->hide();
        w->updateGeometry(getScreenSize());
        return w;
    }

    std::unique_ptr<Bar> TopLevelWindow::createBar(const std::string &name)
    {
        std::unique_ptr<Bar> b = std::make_unique<Bar>(name, *this);
        b->hide();
        b->updateGeometry(getScreenSize());
        return b;
    }

    std::unique_ptr<Button> TopLevelWindow::createButton(const std::string &name)
    {
        std::unique_ptr<Button> b = std::make_unique<Button>(name, *this);
        b->hide();
        b->updateGeometry(getScreenSize());
        return b;
    }

    std::unique_ptr<Checkbox> TopLevelWindow::createCheckbox(const std::string &name)
    {
        std::unique_ptr<Checkbox> c = std::make_unique<Checkbox>(name, *this);
        c->hide();
        c->updateGeometry(getScreenSize());
        return c;
    }

    std::unique_ptr<Combobox> TopLevelWindow::createCombobox(const std::string &name)
    {
        std::unique_ptr<Combobox> c = std::make_unique<Combobox>(name, *this);
        c->hide();
        c->updateGeometry(getScreenSize());
        return c;
    }

    std::unique_ptr<Image> TopLevelWindow::createImage(const std::string &name)
    {
        std::unique_ptr<Image> i = std::make_unique<Image>(name, *this);
        i->hide();
        i->updateGeometry(getScreenSize());
        return i;
    }

    std::unique_ptr<Label> TopLevelWindow::createLabel(const std::string &name)
    {
        std::unique_ptr<Label> l = std::make_unique<Label>(name, *this);
        l->hide();
        l->updateGeometry(getScreenSize());
        return l;
    }

    std::unique_ptr<SceneWindow> TopLevelWindow::createSceneWindow(const std::string &name)
    {
        std::unique_ptr<SceneWindow> s = std::make_unique<SceneWindow>(name, *this);
        s->hide();
        s->updateGeometry(getScreenSize());
        return s;
    }

    std::unique_ptr<TabWidget> TopLevelWindow::createTabWidget(const std::string &name)
    {
        std::unique_ptr<TabWidget> t = std::make_unique<TabWidget>(name, *this);
        t->hide();
        t->updateGeometry(getScreenSize());
        return t;
    }

    std::unique_ptr<Textbox> TopLevelWindow::createTextbox(const std::string &name)
    {
        std::unique_ptr<Textbox> t = std::make_unique<Textbox>(name, *this);
        t->hide();
        t->updateGeometry(getScreenSize());
        return t;
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

    /*KeyValueMapList TopLevelWindow::maps()
		{
			return Scope::maps().merge(mTopLevelWidgets, mUI);
		}*/

    Input::InputHandler *TopLevelWindow::input()
    {
        return mExternalInput ? mExternalInput : *mInputHandlerSelector;
    }

    void TopLevelWindow::addOverlay(WindowOverlay *overlay)
    {
        mOverlays.push_back(overlay);
    }

    void TopLevelWindow::renderOverlays()
    {
        for (WindowOverlay *overlay : mOverlays)
            overlay->render();
    }

    bool TopLevelWindow::injectKeyPress(const Input::KeyEventArgs &arg)
    {
        for (WindowOverlay *overlay : mOverlays) {
            if (overlay->injectKeyPress(arg))
                return true;
        }
        return false;
    }

    bool TopLevelWindow::injectKeyRelease(const Input::KeyEventArgs &arg)
    {
        for (WindowOverlay *overlay : mOverlays) {
            if (overlay->injectKeyRelease(arg))
                return true;
        }
        return false;
    }

    static bool propagateInput(WidgetBase *w, const Input::PointerEventArgs &arg, const Vector3 &screenSize, const Vector3 &screenPos, bool (WidgetBase::*f)(const Input::PointerEventArgs &))
    {
        if (!w->mVisible)
            return false;

        if (!w->containsPoint(arg.position, screenSize, screenPos))
            return false;

        for (WidgetBase *c : w->children()) {
            if (propagateInput(c, arg, screenSize, screenPos, f))
                return true;
        }
        return (w->*f)(arg);
    }

    bool TopLevelWindow::injectPointerPress(const Input::PointerEventArgs &arg)
    {
        for (WindowOverlay *overlay : mOverlays) {
            if (overlay->injectPointerPress(arg))
                return true;
        }

        auto [screenPos, screenSize] = getAvailableScreenSpace();

        for (WidgetBase *w : uniquePtrToPtr(mTopLevelWidgets)) {
            if (propagateInput(w, arg, screenSize, screenPos, &WidgetBase::injectPointerPress))
                return true;
        }
        return false;
    }

    bool TopLevelWindow::injectPointerRelease(const Input::PointerEventArgs &arg)
    {
        for (WindowOverlay *overlay : mOverlays) {
            if (overlay->injectPointerRelease(arg))
                return true;
        }

        auto [screenPos, screenSize] = getAvailableScreenSpace();

        for (WidgetBase *w : uniquePtrToPtr(mTopLevelWidgets)) {
            if (propagateInput(w, arg, screenSize, screenPos, &WidgetBase::injectPointerRelease))
                return true;
        }
        return false;
    }

    static WidgetBase *getHoveredWidgetUp(const Vector2 &pos, const Vector3 &screenSize, WidgetBase *current)
    {
        if (!current) {
            return nullptr;
        } else if (!current->mVisible || !current->containsPoint(pos, screenSize)) {
            return getHoveredWidgetUp(pos, screenSize, current->getParent());
        } else {
            return current;
        }
    }

    WidgetBase *TopLevelWindow::getHoveredWidgetDown(const Vector2 &pos, const Vector3 &screenSize, WidgetBase *current)
    {
        const auto &widgets = current ? current->children() : uniquePtrToPtr(static_cast<const std::vector<std::unique_ptr<WidgetBase>> &>(mTopLevelWidgets));

        for (WidgetBase *w : widgets) {
            if (w->mVisible && w->containsPoint(pos, screenSize)) {
                return getHoveredWidgetDown(pos, screenSize, w);
            }
        }
        return current;
    }

    WidgetBase *TopLevelWindow::getHoveredWidget(const Vector2 &pos, const Vector3 &screenSize, WidgetBase *current)
    {
        return getHoveredWidgetDown(pos, screenSize, getHoveredWidgetUp(pos, screenSize, current));
    }

    bool TopLevelWindow::injectPointerMove(const Input::PointerEventArgs &arg)
    {
        auto [screenPos, screenSize] = getAvailableScreenSpace();

        for (WindowOverlay *overlay : mOverlays) {
            if (overlay->injectPointerMove(arg))
                return true;
        }

        Vector2 mouse = arg.position - screenPos.xy() - Vector2 { static_cast<float>(mWindow->renderX()), static_cast<float>(mWindow->renderY()) };

        if (std::find_if(mWidgets.begin(), mWidgets.end(), [&](const std::pair<const std::string, WidgetBase *> &p) { return p.second == mHoveredWidget; }) == mWidgets.end())
            mHoveredWidget = nullptr;

        WidgetBase *hoveredWidget = getHoveredWidget(mouse, screenSize, mHoveredWidget);

        if (mHoveredWidget != hoveredWidget) {

            if (mHoveredWidget)
                mHoveredWidget->injectPointerLeave(arg);

            mHoveredWidget = hoveredWidget;

            if (mHoveredWidget)
                mHoveredWidget->injectPointerEnter(arg);
        }

        if (mHoveredWidget)
            return mHoveredWidget->injectPointerMove(arg);

        return false;
    }

    Window::Window *TopLevelWindow::window() const
    {
        return mWindow;
    }

    WidgetBase *TopLevelWindow::currentRoot()
    {
        return mCurrentRoot;
    }

    Render::RenderWindow *TopLevelWindow::getRenderer()
    {
        return mRenderWindow.get();
    }

    void TopLevelWindow::onClose()
    {
        close();
    }

    void TopLevelWindow::onRepaint()
    {
        //update();
    }

    void TopLevelWindow::onResize(size_t width, size_t height)
    {
        input()->onResize(width, height);
        for (WidgetBase *topLevel : uniquePtrToPtr(mTopLevelWidgets)) {
            topLevel->screenSizeChanged({ static_cast<float>(width), static_cast<float>(height), 1.0f });
        }
    }

    void TopLevelWindow::calculateWindowGeometries()
    {
        for (WidgetBase *topLevel : uniquePtrToPtr(mTopLevelWidgets)) {
            topLevel->updateGeometry(getScreenSize());
        }
    }

    void TopLevelWindow::destroyTopLevel(WidgetBase *w)
    {
        auto it = std::find_if(mTopLevelWidgets.begin(), mTopLevelWidgets.end(), [=](const std::unique_ptr<WidgetBase> &ptr) { return ptr.get() == w; });
        assert(it != mTopLevelWidgets.end());
        mTopLevelWidgets.erase(it);
    }

    void TopLevelWindow::clear()
    {
        mTopLevelWidgets.clear();
    }

    GUISystem &TopLevelWindow::gui()
    {
        return mGui;
    }

    bool TopLevelWindow::isHovered(WidgetBase *w)
    {
        WidgetBase *hovered = mHoveredWidget;
        while (hovered) {
            if (hovered == w)
                return true;
            hovered = hovered->getParent();
        }
        return false;
    }

    WidgetBase *TopLevelWindow::hoveredWidget()
    {
        return mHoveredWidget;
    }

    WidgetBase *TopLevelWindow::getWidget(const std::string &name)
    {
        auto it = mWidgets.find(name);
        if (it == mWidgets.end())
            return nullptr;
        return it->second;
    }

    void TopLevelWindow::registerWidget(WidgetBase *w)
    {
        if (!w->getName().empty()) {
            auto pib = mWidgets.try_emplace(w->getName(), w);
            assert(pib.second);
        }
    }

    void TopLevelWindow::unregisterWidget(WidgetBase *w)
    {
        if (!w->getName().empty()) {
            size_t result = mWidgets.erase(w->getName());
            assert(result == 1);
        }
    }

    void TopLevelWindow::swapCurrentRoot(WidgetBase *newRoot)
    {
        if (mCurrentRoot)
            mCurrentRoot->hide();
        mCurrentRoot = newRoot;
        newRoot->show();
    }

    void TopLevelWindow::openModalWidget(WidgetBase *widget)
    {
        widget->showModal();
        mModalWidgetList.emplace(widget);
    }

    void TopLevelWindow::openWidget(WidgetBase *widget)
    {
        widget->show();
    }

    void TopLevelWindow::closeModalWidget(WidgetBase *widget)
    {
        assert(mModalWidgetList.size() > 0 && mModalWidgetList.top() == widget);
        widget->hideModal();
        mModalWidgetList.pop();
        if (mModalWidgetList.size() > 0)
            mModalWidgetList.top()->activate();
        else if (mCurrentRoot)
            mCurrentRoot->activate();
    }

    void TopLevelWindow::closeWidget(WidgetBase *widget)
    {
        widget->hide();
    }

    bool TopLevelWindow::frameStarted(std::chrono::microseconds)
    {
        mRenderWindow->beginFrame();
        return true;
    }

    bool TopLevelWindow::frameRenderingQueued(std::chrono::microseconds, Scene::ContextMask)
    {
        mRenderWindow->render();
        return true;
    }

    bool TopLevelWindow::frameEnded(std::chrono::microseconds)
    {
        PROFILE();
        renderOverlays();
        mRenderWindow->endFrame();
        return true;
    }

    TopLevelWindowComponentBase::TopLevelWindowComponentBase(TopLevelWindow &window)
        : mWindow(window)
    {
    }

    TopLevelWindow &TopLevelWindowComponentBase::window() const
    {
        return mWindow;
    }

    const MadgineObject *TopLevelWindowComponentBase::parent() const
    {
        return &mWindow.gui();
    }

    TopLevelWindowComponentBase &Engine::GUI::TopLevelWindow::getWindowComponent(size_t i, bool init)
    {
        TopLevelWindowComponentBase &component = mComponents.get(i);
        if (init) {
            component.callInit();
        }
        return component.getSelf(init);
    }

    TopLevelWindowComponentBase &TopLevelWindowComponentBase::getSelf(bool init)
    {
        if (init) {
            checkDependency();
        }
        return *this;
    }

    /*App::Application &TopLevelWindowComponentBase::app(bool init)
    {
        if (init) {
            checkDependency();
        }
        return mWindow.gui().app(init);
    }*/

}
}

DEFINE_UNIQUE_COMPONENT(Engine::GUI, TopLevelWindow)

METATABLE_BEGIN(Engine::GUI::TopLevelWindow)
READONLY_PROPERTY(Widgets, widgets)
READONLY_PROPERTY(Components, components)
METATABLE_END(Engine::GUI::TopLevelWindow)

RegisterType(Engine::GUI::TopLevelWindow);
RegisterType(Engine::GUI::TopLevelWindowComponentBase);