#include "../widgetslib.h"

#include "widgetmanager.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "meshloader.h"
#include "programloader.h"

#include "widget.h"

#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "image.h"
#include "label.h"
#include "scenewindow.h"
#include "tabwidget.h"
#include "textbox.h"

#include "imagedata.h"

#include "vertex.h"

#include "Madgine/gui/toplevelwindow.h"

#include "Interfaces/window/windowapi.h"

UNIQUECOMPONENT(Engine::Widgets::WidgetManager)

METATABLE_BEGIN(Engine::Widgets::WidgetManager)
READONLY_PROPERTY(Widgets, widgets)
MEMBER(mStartupWidget)
METATABLE_END(Engine::Widgets::WidgetManager)

SERIALIZETABLE_BEGIN(Engine::Widgets::WidgetManager)
FIELD(mStartupWidget)
FIELD(mTopLevelWidgets)
SERIALIZETABLE_END(Engine::Widgets::WidgetManager)

RegisterType(Engine::Widgets::WidgetManager);

namespace Engine {
namespace Widgets {

    WidgetManager::WidgetManager(GUI::TopLevelWindow &window)
        : VirtualScope(window, 20)
        , mUIAtlas({ 512, 512 })
    {
    }

    WidgetManager::~WidgetManager()
    {
        assert(mWidgets.empty());
    }

    std::string_view WidgetManager::key() const
    {
        return "WidgetManager";
    }

    bool WidgetManager::init()
    {
        mProgram.create("ui");

        mMesh = Render::MeshLoader::loadManual("widgetMesh", {}, [](Render::MeshLoader *loader, Render::MeshData &mesh, Render::MeshLoader::ResourceType *res) {
            return loader->generate<GUI::Vertex>(mesh, 3, {});
        });

        mUIAtlasTexture.create("widgetUIAtlas", Render::FORMAT_FLOAT8);

        mWindow.getRenderWindow()->addRenderPass(this);

        /*WidgetBase *loading = createTopLevelImage("Loading");
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
        WidgetBase *lobbyList = lobbyListMenu->createChildCombobox("LobbyList");*/

        return true;
    }

    void WidgetManager::finalize()
    {
        mWindow.getRenderWindow()->removeRenderPass(this);

        mTopLevelWidgets.clear();

        /*mDefaultTexture.reset();
        mUIAtlasTexture.reset();*/
    }

    WidgetBase *WidgetManager::createTopLevelWidget(const std::string &name)
    {
        std::unique_ptr<WidgetBase> w = createWidget(name);
        return mTopLevelWidgets.emplace_back(std::move(w))->get();
    }

    Bar *WidgetManager::createTopLevelBar(const std::string &name)
    {
        std::unique_ptr<Bar> p = createBar(name);
        Bar *b = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return b;
    }

    Button *WidgetManager::createTopLevelButton(const std::string &name)
    {
        std::unique_ptr<Button> p = createButton(name);
        Button *b = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return b;
    }

    Checkbox *WidgetManager::createTopLevelCheckbox(const std::string &name)
    {
        std::unique_ptr<Checkbox> p = createCheckbox(name);
        Checkbox *c = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return c;
    }

    Combobox *WidgetManager::createTopLevelCombobox(const std::string &name)
    {
        std::unique_ptr<Combobox> p = createCombobox(name);
        Combobox *c = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return c;
    }

    Image *WidgetManager::createTopLevelImage(const std::string &name)
    {
        std::unique_ptr<Image> p = createImage(name);
        Image *i = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return i;
    }

    Label *WidgetManager::createTopLevelLabel(const std::string &name)
    {
        std::unique_ptr<Label> p = createLabel(name);
        Label *l = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return l;
    }

    SceneWindow *WidgetManager::createTopLevelSceneWindow(const std::string &name)
    {
        std::unique_ptr<SceneWindow> p = createSceneWindow(name);
        SceneWindow *s = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return s;
    }

    TabWidget *WidgetManager::createTopLevelTabWidget(const std::string &name)
    {
        std::unique_ptr<TabWidget> p = createTabWidget(name);
        TabWidget *t = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return t;
    }

    Textbox *WidgetManager::createTopLevelTextbox(const std::string &name)
    {
        std::unique_ptr<Textbox> p = createTextbox(name);
        Textbox *t = p.get();
        mTopLevelWidgets.emplace_back(std::move(p));
        return t;
    }

    std::unique_ptr<WidgetBase> WidgetManager::createWidgetClass(const std::string &name, WidgetClass _class)
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
            std::terminate();
        }
    }

    std::tuple<std::unique_ptr<WidgetBase>> WidgetManager::createWidgetClassTuple(const std::string &name, WidgetClass _class)
    {
        return { createWidgetClass(name, _class) };
    }

    std::tuple<std::pair<const char *, std::string>, std::pair<const char *, WidgetClass>> WidgetManager::storeWidgetCreationData(const std::unique_ptr<WidgetBase> &widget) const
    {
        return std::make_tuple(
            std::make_pair("name", widget->getName()),
            std::make_pair("type", widget->getClass()));
    }

    std::unique_ptr<WidgetBase> WidgetManager::createWidget(const std::string &name)
    {
        std::unique_ptr<WidgetBase> w = std::make_unique<WidgetBase>(name, *this);
        w->hide();
        w->updateGeometry(mClientSpace);
        return w;
    }

    std::unique_ptr<Bar> WidgetManager::createBar(const std::string &name)
    {
        std::unique_ptr<Bar> b = std::make_unique<Bar>(name, *this);
        b->hide();
        b->updateGeometry(mClientSpace);
        return b;
    }

    std::unique_ptr<Button> WidgetManager::createButton(const std::string &name)
    {
        std::unique_ptr<Button> b = std::make_unique<Button>(name, *this);
        b->hide();
        b->updateGeometry(mClientSpace);
        return b;
    }

    std::unique_ptr<Checkbox> WidgetManager::createCheckbox(const std::string &name)
    {
        std::unique_ptr<Checkbox> c = std::make_unique<Checkbox>(name, *this);
        c->hide();
        c->updateGeometry(mClientSpace);
        return c;
    }

    std::unique_ptr<Combobox> WidgetManager::createCombobox(const std::string &name)
    {
        std::unique_ptr<Combobox> c = std::make_unique<Combobox>(name, *this);
        c->hide();
        c->updateGeometry(mClientSpace);
        return c;
    }

    std::unique_ptr<Image> WidgetManager::createImage(const std::string &name)
    {
        std::unique_ptr<Image> i = std::make_unique<Image>(name, *this);
        i->hide();
        i->updateGeometry(mClientSpace);
        return i;
    }

    std::unique_ptr<Label> WidgetManager::createLabel(const std::string &name)
    {
        std::unique_ptr<Label> l = std::make_unique<Label>(name, *this);
        l->hide();
        l->updateGeometry(mClientSpace);
        return l;
    }

    std::unique_ptr<SceneWindow> WidgetManager::createSceneWindow(const std::string &name)
    {
        std::unique_ptr<SceneWindow> s = std::make_unique<SceneWindow>(name, *this);
        s->hide();
        s->updateGeometry(mClientSpace);
        return s;
    }

    std::unique_ptr<TabWidget> WidgetManager::createTabWidget(const std::string &name)
    {
        std::unique_ptr<TabWidget> t = std::make_unique<TabWidget>(name, *this);
        t->hide();
        t->updateGeometry(mClientSpace);
        return t;
    }

    std::unique_ptr<Textbox> WidgetManager::createTextbox(const std::string &name)
    {
        std::unique_ptr<Textbox> t = std::make_unique<Textbox>(name, *this);
        t->hide();
        t->updateGeometry(mClientSpace);
        return t;
    }

    static bool propagateInput(WidgetBase *w, const Input::PointerEventArgs &arg, const Rect2i &screenSpace, bool (WidgetBase::*f)(const Input::PointerEventArgs &))
    {
        if (!w->mVisible)
            return false;

        if (!w->containsPoint(arg.position, screenSpace))
            return false;

        for (WidgetBase *c : w->children()) {
            if (propagateInput(c, arg, screenSpace, f))
                return true;
        }
        return (w->*f)(arg);
    }

    bool WidgetManager::injectPointerPress(const Input::PointerEventArgs &arg)
    {

        Input::PointerEventArgs modArgs = arg;
        modArgs.position -= Vector2 { getScreenSpace().mTopLeft };

        Rect2i clientSpace = mClientSpace;

        for (WidgetBase *modalWidget : mModalWidgetList) {
            if (propagateInput(modalWidget, modArgs, clientSpace, &WidgetBase::injectPointerPress))
                return true;
        }

        for (WidgetBase *w : uniquePtrToPtr(mTopLevelWidgets)) {
            if (propagateInput(w, modArgs, clientSpace, &WidgetBase::injectPointerPress))
                return true;
        }

        return false;
    }

    bool WidgetManager::injectPointerRelease(const Input::PointerEventArgs &arg)
    {

        Input::PointerEventArgs modArgs = arg;
        modArgs.position -= Vector2 { Vector2i { mWindow.window()->renderX(), mWindow.window()->renderY() } + mClientSpace.mTopLeft };

        Rect2i clientSpace = { { 0, 0 }, mClientSpace.mSize };

        for (WidgetBase *modalWidget : mModalWidgetList) {
            if (propagateInput(modalWidget, modArgs, clientSpace, &WidgetBase::injectPointerRelease))
                return true;
        }

        for (WidgetBase *w : uniquePtrToPtr(mTopLevelWidgets)) {
            if (propagateInput(w, modArgs, clientSpace, &WidgetBase::injectPointerRelease))
                return true;
        }

        return false;
    }

    static WidgetBase *getHoveredWidgetUp(const Vector2 &pos, const Rect2i &screenSpace, WidgetBase *current)
    {
        if (!current) {
            return nullptr;
        } else if (!current->mVisible || !current->containsPoint(pos, screenSpace)) {
            return getHoveredWidgetUp(pos, screenSpace, current->getParent());
        } else {
            return current;
        }
    }

    WidgetBase *WidgetManager::getHoveredWidgetDown(const Vector2 &pos, const Rect2i &screenSpace, WidgetBase *current)
    {
        static auto &logOnce = LOG_WARNING("Handle modal widgets for hover");

        const auto &widgets = current ? current->children() : uniquePtrToPtr(static_cast<const std::vector<std::unique_ptr<WidgetBase>> &>(mTopLevelWidgets));

        for (WidgetBase *w : widgets) {
            if (w->mVisible && w->containsPoint(pos, screenSpace)) {
                return getHoveredWidgetDown(pos, screenSpace, w);
            }
        }
        return current;
    }

    WidgetBase *WidgetManager::getHoveredWidget(const Vector2 &pos, const Rect2i &screenSpace, WidgetBase *current)
    {
        return getHoveredWidgetDown(pos, screenSpace, getHoveredWidgetUp(pos, screenSpace, current));
    }

    bool WidgetManager::injectPointerMove(const Input::PointerEventArgs &arg)
    {
        Input::PointerEventArgs modArgs = arg;
        modArgs.position -= Vector2 { static_cast<float>(mWindow.window()->renderX()), static_cast<float>(mWindow.window()->renderY()) };

        Rect2i clientSpace = mClientSpace;

        Vector2 mouse = modArgs.position - clientSpace.mTopLeft;

        if (std::find_if(mWidgets.begin(), mWidgets.end(), [&](const std::pair<const std::string, WidgetBase *> &p) { return p.second == mHoveredWidget; }) == mWidgets.end())
            mHoveredWidget = nullptr;

        WidgetBase *hoveredWidget = getHoveredWidget(mouse, clientSpace, mHoveredWidget);

        if (mHoveredWidget != hoveredWidget) {

            if (mHoveredWidget)
                mHoveredWidget->injectPointerLeave(modArgs);

            mHoveredWidget = hoveredWidget;

            if (mHoveredWidget)
                mHoveredWidget->injectPointerEnter(modArgs);
        }

        if (mHoveredWidget)
            return mHoveredWidget->injectPointerMove(modArgs);

        return false;
    }

    WidgetBase *WidgetManager::currentRoot()
    {
        return mCurrentRoot;
    }

    void WidgetManager::destroyTopLevel(WidgetBase *w)
    {
        auto it = std::find_if(mTopLevelWidgets.begin(), mTopLevelWidgets.end(), [=](const std::unique_ptr<WidgetBase> &ptr) { return ptr.get() == w; });
        assert(it != mTopLevelWidgets.end());
        mTopLevelWidgets.erase(it);
    }

    void WidgetManager::clear()
    {
        mTopLevelWidgets.clear();
    }

    bool WidgetManager::isHovered(WidgetBase *w)
    {
        WidgetBase *hovered = mHoveredWidget;
        while (hovered) {
            if (hovered == w)
                return true;
            hovered = hovered->getParent();
        }
        return false;
    }

    WidgetBase *WidgetManager::hoveredWidget()
    {
        return mHoveredWidget;
    }

    WidgetBase *WidgetManager::getWidget(const std::string &name)
    {
        auto it = mWidgets.find(name);
        if (it == mWidgets.end())
            return nullptr;
        return it->second;
    }

    void WidgetManager::registerWidget(WidgetBase *w)
    {
        if (!w->getName().empty()) {
            mWidgets.try_emplace(w->getName(), w);
        }
    }

    void WidgetManager::updateWidget(WidgetBase *w, const std::string &newName)
    {
        unregisterWidget(w);
        if (!newName.empty()) {
            mWidgets.try_emplace(newName, w);
        }
    }

    void WidgetManager::unregisterWidget(WidgetBase *w)
    {
        if (!w->getName().empty()) {
            auto it = mWidgets.find(w->getName());
            assert(it != mWidgets.end());
            if (it->second == w) {
                mWidgets.erase(it);
            }
        }
    }

    void WidgetManager::swapCurrentRoot(WidgetBase *newRoot)
    {
        if (mCurrentRoot)
            mCurrentRoot->hide();
        mCurrentRoot = newRoot;
        newRoot->show();
    }

    void WidgetManager::openModalWidget(WidgetBase *widget)
    {
        widget->show();
        mModalWidgetList.emplace(mModalWidgetList.begin(), widget);
    }

    void WidgetManager::openWidget(WidgetBase *widget)
    {
        widget->show();
    }

    void WidgetManager::closeModalWidget(WidgetBase *widget)
    {
        assert(mModalWidgetList.size() > 0 && mModalWidgetList.front() == widget);
        widget->hide();
        mModalWidgetList.erase(mModalWidgetList.begin());
    }

    void WidgetManager::closeWidget(WidgetBase *widget)
    {
        widget->hide();
    }

    void WidgetManager::openStartupWidget()
    {
        if (mStartupWidget)
            swapCurrentRoot(mStartupWidget);
    }

    void WidgetManager::onResize(const Rect2i &space)
    {
        TopLevelWindowComponentBase::onResize(space);
        for (WidgetBase *topLevel : uniquePtrToPtr(mTopLevelWidgets)) {
            topLevel->updateGeometry(space);
        }
    }

    void WidgetManager::render(Render::RenderTarget *target)
    {
        Rect2i screenSpace = mClientSpace;

        target->setRenderSpace(screenSpace);

        std::map<Render::TextureDescriptor, std::vector<GUI::Vertex>> vertices;

        std::queue<Widgets::WidgetBase *> q;
        for (Widgets::WidgetBase *w : widgets()) {
            if (w->mVisible) {
                q.push(w);
            }
        }
        while (!q.empty()) {

            Widgets::WidgetBase *w = q.front();            
            q.pop();

            for (Widgets::WidgetBase *c : w->children()) {
                if (c->mVisible)
                    q.push(c);
            }

            std::vector<std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor>> localVerticesList = w->vertices(Vector3 { Vector2 { screenSpace.mSize }, 1.0f });

            Resources::ImageLoader::ResourceType *resource = w->resource();
            if (resource) {
                auto it = mUIAtlasEntries.find(resource);
                if (it == mUIAtlasEntries.end()) {
                    Resources::ImageLoader::HandleType data = resource->loadData();
                    it = mUIAtlasEntries.try_emplace(resource, mUIAtlas.insert({ data->mWidth, data->mHeight }, [this]() { expandUIAtlas(); })).first;
                    mUIAtlasTexture.setSubData({ it->second.mArea.mTopLeft.x, it->second.mArea.mTopLeft.y }, it->second.mArea.mSize, { data->mBuffer, static_cast<size_t>(data->mWidth * data->mHeight) });
                }

                for (std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor> &localVertices : localVerticesList) {

                    std::transform(localVertices.first.begin(), localVertices.first.end(), std::back_inserter(vertices[localVertices.second]), [&](const GUI::Vertex &v) {
                        return GUI::Vertex {
                            v.mPos,
                            v.mColor,
                            { (it->second.mArea.mSize.x / (512.f * mUIAtlasSize)) * v.mUV.x + it->second.mArea.mTopLeft.x / (512.f * mUIAtlasSize),
                                (it->second.mArea.mSize.y / (512.f * mUIAtlasSize)) * v.mUV.y + it->second.mArea.mTopLeft.y / (512.f * mUIAtlasSize) }
                        };
                    });
                }

            } else {
                for (std::pair<std::vector<GUI::Vertex>, Render::TextureDescriptor> &localVertices : localVerticesList) {
                    std::move(localVertices.first.begin(), localVertices.first.end(), std::back_inserter(vertices[localVertices.second]));
                }
            }
        }

        for (std::pair<const Render::TextureDescriptor, std::vector<GUI::Vertex>> &p : vertices) {
            if (!p.second.empty()) {

                mParameters.hasDistanceField = bool(p.first.mFlags & Render::TextureFlag_IsDistanceField);
                mProgram.setParameters(mParameters, 2);

                if (p.first.mTextureHandle)
                    Render::TextureLoader::getSingleton().bind(p.first.mTextureHandle);
                else
                    mUIAtlasTexture.bind();

                mMesh.update(3, std::move(p.second));

                target->renderMesh(mMesh, mProgram);
            }
        }
    }

    void WidgetManager::expandUIAtlas()
    {
        if (mUIAtlasSize == 0) {
            mUIAtlasSize = 4;
            mUIAtlasTexture.setData({ mUIAtlasSize * 512, mUIAtlasSize * 512 }, {});
            for (int x = 0; x < mUIAtlasSize; ++x) {
                for (int y = 0; y < mUIAtlasSize; ++y) {
                    mUIAtlas.addBin({ 512 * x, 512 * y });
                }
            }
        } else {
            /*for (int x = 0; x < mUIAtlasSize; ++x) {
                for (int y = 0; y < mUIAtlasSize; ++y) {
                    mUIAtlas.addBin({ 512 * x, 512 * (y + mUIAtlasSize) });
                    mUIAtlas.addBin({ 512 * (x + mUIAtlasSize), 512 * y });
                    mUIAtlas.addBin({ 512 * (x + mUIAtlasSize), 512 * (y + mUIAtlasSize) });
                }
            }
            mUIAtlasSize *= 2;
            mUIAtlasTexture.resize({ 512 * mUIAtlasSize, 512 * mUIAtlasSize });*/
            throw "TODO";
        }
    }

    Render::Texture &WidgetManager::uiTexture() const
    {
        return *mUIAtlasTexture;
    }

    int WidgetManager::priority() const
    {
        return mPriority;
    }

}
}