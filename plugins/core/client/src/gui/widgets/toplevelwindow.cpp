#include "../../clientlib.h"

#include "toplevelwindow.h"

#include "../../ui/uimanager.h"

#include "bar.h"
#include "button.h"
#include "checkbox.h"
#include "combobox.h"
#include "label.h"
#include "scenewindow.h"
#include "tabwidget.h"
#include "textbox.h"
#include "image.h"

#include "../guisystem.h"

#include "../../input/inputhandler.h"

#include "Interfaces/generic/keyvalueiterate.h"

#include "Interfaces/window/windowapi.h"

#include "Madgine/app/application.h"
#include "Madgine/app/appsettings.h"

#include "../windowoverlay.h"

#include "../../render/renderwindow.h"

#include "Interfaces/generic/transformIt.h"

#include "Madgine/scene/scenemanager.h"

namespace Engine
{	

	namespace GUI
	{
		TopLevelWindow::TopLevelWindow(GUISystem& gui) :
			Scope(&gui),
			mGui(gui),
			mUI(std::make_unique<UI::UIManager>(*this))
		{
			const App::AppSettings &settings = gui.app().settings();

			mWindow = Window::sCreateWindow(settings.mWindowSettings);

			mWindow->addListener(this);


			if (settings.mInput) {
				mExternalInput = static_cast<Input::InputHandler*>(settings.mInput);
			}
			else {
				mInputHandlerSelector.emplace(mWindow);
			}
			input()->setListener(this);
			gui.app(false).addFrameListener(input());

			mRenderWindow = gui.renderer().createWindow(this);

			Widget *loading = createTopLevelImage("Loading");
			Widget *progress = loading->createChildBar("ProgressBar");
			progress->setSize({ 0.8f,0,0,0,0.1f,0,0,0,1 });
			progress->setPos({ 0.1f,0,0,0,0.85f,0,0,0,0 });
			Widget *loadMsg = loading->createChildLabel("LoadingMsg");
			Widget *ingame = createTopLevelWidget("ingame");
			SceneWindow *game = ingame->createChildSceneWindow("game");
			//game->setCamera(gui.sceneMgr().createCamera());
			game->setSize({ 0.8f,0,0,0,1,0,0,0,1 });
			/*Widget *placeBaseButton = game->createChildButton("PlaceBaseButton");
			placeBaseButton->setPos({ 1,0,-120,0,0,30,0,0,0 });
			placeBaseButton->setSize({ 0,0,100,0,0,30,0,0,1 });*/
			Widget *endTurnButton = game->createChildButton("EndTurnButton");
			endTurnButton->setPos({ 1,0,-120,0,0,30,0,0,0 });
			endTurnButton->setSize({ 0,0,100,0,0,30,0,0,1 });
			Widget *unitUI = ingame->createChildWidget("UnitUI");
			unitUI->setPos({ 0.8f,0,0,0,0,0,0,0,0 });
			unitUI->setSize({ 0.2f,0,0,0,1,0,0,0,1 });
			Widget *fieldImage = unitUI->createChildImage("FieldImage");
			fieldImage->setPos({ 0, 0, 0, 0, 0, 0, 0, 0, 0 });
			fieldImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
			Widget *defenderImage = unitUI->createChildImage("DefenderImage");
			defenderImage->setPos({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 0 });
			defenderImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
			Widget *defenderHealth = defenderImage->createChildBar("DefenderHealth");
			defenderHealth->setPos({ 0, 0, 0, 0, 0.85f, 0, 0, 0, 0 });
			defenderHealth->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *defenderExp = defenderImage->createChildBar("DefenderExp");
			defenderExp->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			defenderExp->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *attackerImage = unitUI->createChildImage("AttackerImage");
			attackerImage->setPos({ 0, 0, 0, 0.5f, 0, 0, 0, 0, 0 });
			attackerImage->setSize({ 0.5f, 0, 0, 0.5f, 0, 0, 0, 0, 1 });
			Widget *attackerHealth = attackerImage->createChildBar("AttackerHealth");
			attackerHealth->setPos({ 0, 0, 0, 0, 0.85f, 0, 0, 0, 0 });
			attackerHealth->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *attackerExp = attackerImage->createChildBar("AttackerExp");
			attackerExp->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			attackerExp->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *modifierImage = unitUI->createChildImage("ModifierImage");
			modifierImage->setPos({ 1.15f, 0, 0, 0.15f, 0, 0, 0, 0, 0 });
			modifierImage->setSize({ 0.2f, 0, 0, 0.2f, 0, 0, 0, 0, 1 });
			Image *attackerStrengthIcon = unitUI->createChildImage("AttackerStrengthIcon");
			attackerStrengthIcon->setPos({ 0.025f, 0, 0, 1.025f, 0, 0, 0, 0, 0 });
			attackerStrengthIcon->setSize({ 0.15f, 0, 0, 0.15f, 0, 0, 0, 0, 1 });
			attackerStrengthIcon->setImage("Sword-01.png");
			Widget *attackerStrength = unitUI->createChildLabel("AttackerStrength");
			attackerStrength->setPos({ 0.2f, 0, 0, 1.025f, 0, 0, 0, 0, 0 });
			attackerStrength->setSize({ 0.2f, 0, 0, 0.15f, 0, 0, 0, 0, 1 });
			Widget *strengthArrow = unitUI->createChildImage("StrengthArrow");
			strengthArrow->setPos({ 0.4f, 0, 0, 1.075f, 0, 0, 0, 0, 0 });
			strengthArrow->setSize({ 0.2f, 0, 0, 0.05f, 0, 0, 0, 0, 1 });
			Widget *defenderStrength = unitUI->createChildLabel("DefenderStrength");
			defenderStrength->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			defenderStrength->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *defenderStrengthIcon = unitUI->createChildImage("DefenderStrengthIcon");
			defenderStrengthIcon->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			defenderStrengthIcon->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *contentWindow = unitUI->createChildWidget("ContentWindow");
			contentWindow->setPos({ 0, 0, 0, 1, 0, 0, 0, 0, 0 });
			contentWindow->setSize({ 1, 0, 0, -1, 1, 0, 0, 0, 1 });
			Widget *movementIcon = unitUI->createChildImage("MovementIcon");
			movementIcon->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			movementIcon->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *movement = unitUI->createChildLabel("Movement");
			movement->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			movement->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *health = unitUI->createChildLabel("Health");
			health->setPos({ 0, 0, 0, 0, 0.7f, 0, 0, 0, 0 });
			health->setSize({ 1, 0, 0, 0, 0.15f, 0, 0, 0, 1 });
			Widget *mainMenu = createTopLevelWidget("MainMenu");
			Widget *optionsButton = mainMenu->createChildButton("OptionsButton");
			optionsButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
			optionsButton->setPos({ 0.55f,0,0,0,0.55f,0,0,0,0 });
			Widget *findMatchButton = mainMenu->createChildButton("FindMatchButton");
			findMatchButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
			findMatchButton->setPos({ 0.55f,0,0,0,0.8f,0,0,0,0 });
			Widget *playButton = mainMenu->createChildButton("PlayButton");
			playButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
			playButton->setPos({ 0.05f,0,0,0,0.55f,0,0,0,0 });
			Widget *quitButton = mainMenu->createChildButton("QuitButton");
			quitButton->setSize({ 0.4f,0,0,0,0.15f,0,0,0,1 });
			quitButton->setPos({ 0.05f,0,0,0,0.8f,0,0,0,0 });
			Image *titleScreen = mainMenu->createChildImage("noname");
			titleScreen->setImage("img/MainMenu/TitleScreen.png");
			titleScreen->setSize({ 0.8f,0,0,0.1f,0,0,0,0,1 });
			titleScreen->setPos({ 0.1f,0,0,0,0.05f,0,0,0,0 });
			Widget *connectionFailureWindow = createTopLevelWidget("ConnectionFailureWindow");
			Widget *connectionLabel = connectionFailureWindow->createChildLabel("ConnectionLabel");
			Widget *retryButton = connectionFailureWindow->createChildButton("RetryButton");
			Widget *abortButton = connectionFailureWindow->createChildButton("AbortButton");
			Widget *lobbyMenu = createTopLevelWidget("LobbyMenu");
			Widget *startGameButton = lobbyMenu->createChildButton("StartGameButton");
			Widget *lobbyListMenu = createTopLevelWidget("LobbyListMenu");
			Widget *createLobbyButton = lobbyListMenu->createChildButton("CreateLobbyButton");
			Widget *joinLobbyButton = lobbyListMenu->createChildButton("JoinLobbyButton");
			Widget *backButton = lobbyListMenu->createChildButton("BackButton");
			Widget *lobbyList = lobbyListMenu->createChildCombobox("LobbyList");
			
		}

		TopLevelWindow::~TopLevelWindow()
		{
			mGui.app(false).removeFrameListener(input());
			mWindow->removeListener(this);

			mUI.reset();

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
				overlay->calculateAvailableScreenSpace(pos, size);
			return std::make_pair(pos, size);
		}

		Matrix3 TopLevelWindow::getSize()
		{
			return Matrix3::IDENTITY;
		}

		Widget* TopLevelWindow::createTopLevelWidget(const std::string& name)
		{
			Widget *w = mTopLevelWidgets.emplace_back(createWidget(name)).get();
			w->hide();
			w->updateGeometry(getScreenSize(), getSize());
			return w;
		}

		Bar* TopLevelWindow::createTopLevelBar(const std::string& name)
		{
			std::unique_ptr<Bar> p = createBar(name);
			Bar *b = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			b->hide();
			b->updateGeometry(getScreenSize(), getSize());
			return b;
		}

		Button* TopLevelWindow::createTopLevelButton(const std::string& name)
		{
			std::unique_ptr<Button> p = createButton(name);
			Button *b = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			b->hide();
			b->updateGeometry(getScreenSize(), getSize());
			return b;
		}

		Checkbox* TopLevelWindow::createTopLevelCheckbox(const std::string& name)
		{
			std::unique_ptr<Checkbox> p = createCheckbox(name);
			Checkbox *c = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			c->hide();
			c->updateGeometry(getScreenSize(), getSize());
			return c;
		}

		Combobox* TopLevelWindow::createTopLevelCombobox(const std::string& name)
		{
			std::unique_ptr<Combobox> p = createCombobox(name);
			Combobox *c = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			c->hide();
			c->updateGeometry(getScreenSize(), getSize());
			return c;
		}

		Image* TopLevelWindow::createTopLevelImage(const std::string& name)
		{
			std::unique_ptr<Image> p = createImage(name);
			Image *i = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			i->hide();
			i->updateGeometry(getScreenSize(), getSize());
			return i;
		}

		Label* TopLevelWindow::createTopLevelLabel(const std::string& name)
		{
			std::unique_ptr<Label> p = createLabel(name);
			Label *l = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			l->hide();
			l->updateGeometry(getScreenSize(), getSize());
			return l;
		}

		SceneWindow* TopLevelWindow::createTopLevelSceneWindow(const std::string& name)
		{
			std::unique_ptr<SceneWindow> p = createSceneWindow(name);
			SceneWindow *s = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			s->hide();
			s->updateGeometry(getScreenSize(), getSize());
			return s;
		}

		TabWidget* TopLevelWindow::createTopLevelTabWidget(const std::string& name)
		{
			std::unique_ptr<TabWidget> p = createTabWidget(name);
			TabWidget *t = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			t->hide();
			t->updateGeometry(getScreenSize(), getSize());
			return t;
		}

		Textbox* TopLevelWindow::createTopLevelTextbox(const std::string& name)
		{
			std::unique_ptr<Textbox> p = createTextbox(name);
			Textbox *t = p.get();
			mTopLevelWidgets.emplace_back(std::move(p));
			t->hide();
			t->updateGeometry(getScreenSize(), getSize());
			return t;
		}

		std::unique_ptr<Widget> TopLevelWindow::createWidgetClass(const std::string& name, Class _class)
		{
			switch (_class)
			{
			case Class::BAR_CLASS:
				return createBar(name);
			case Class::CHECKBOX_CLASS:
				return createCheckbox(name);
			case Class::LABEL_CLASS:
				return createLabel(name);
			case Class::TABWIDGET_CLASS:
				return createTabWidget(name);
			case Class::BUTTON_CLASS:
				return createButton(name);
			case Class::COMBOBOX_CLASS:
				return createCombobox(name);
			case Class::TEXTBOX_CLASS:
				return createTextbox(name);
			case Class::SCENEWINDOW_CLASS:
				return createSceneWindow(name);
			case Class::IMAGE_CLASS:
				return createImage(name);
			default:
				throw 0;
			}
		}

		std::unique_ptr<Widget> TopLevelWindow::createWidget(const std::string & name)
		{
			return std::make_unique<Widget>(name, *this);
		}

		std::unique_ptr<Bar> TopLevelWindow::createBar(const std::string & name)
		{
			return std::make_unique<Bar>(name, *this);
		}

		std::unique_ptr<Button> TopLevelWindow::createButton(const std::string & name)
		{
			return std::make_unique<Button>(name, *this);
		}

		std::unique_ptr<Checkbox> TopLevelWindow::createCheckbox(const std::string & name)
		{
			return std::make_unique<Checkbox>(name, *this);
		}

		std::unique_ptr<Combobox> TopLevelWindow::createCombobox(const std::string & name)
		{
			return std::make_unique<Combobox>(name, *this);
		}

		std::unique_ptr<Image> TopLevelWindow::createImage(const std::string & name)
		{
			return std::make_unique<Image>(name, *this);
		}

		std::unique_ptr<Label> TopLevelWindow::createLabel(const std::string & name)
		{
			return std::make_unique<Label>(name, *this);
		}

		std::unique_ptr<SceneWindow> TopLevelWindow::createSceneWindow(const std::string & name)
		{
			return std::make_unique<SceneWindow>(name, *this);
		}

		std::unique_ptr<TabWidget> TopLevelWindow::createTabWidget(const std::string & name)
		{
			return std::make_unique<TabWidget>(name, *this);
		}

		std::unique_ptr<Textbox> TopLevelWindow::createTextbox(const std::string & name)
		{
			return std::make_unique<Textbox>(name, *this);
		}

		const Core::MadgineObject * TopLevelWindow::parent() const
		{
			return nullptr;
		}

		UI::UIManager & TopLevelWindow::ui()
		{
			return *mUI;
		}

		App::Application & TopLevelWindow::app(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mGui.app(init);
		}

		KeyValueMapList TopLevelWindow::maps()
		{
			return Scope::maps().merge(mTopLevelWidgets, mUI);
		}

		Input::InputHandler* TopLevelWindow::input()
		{
			return mExternalInput ? mExternalInput : *mInputHandlerSelector;
		}

		void TopLevelWindow::addOverlay(WindowOverlay * overlay)
		{
			mOverlays.push_back(overlay);
		}

		void TopLevelWindow::renderOverlays()
		{
			for (WindowOverlay *overlay : mOverlays)
				overlay->render();
		}

		bool TopLevelWindow::injectKeyPress(const Input::KeyEventArgs & arg)
		{
			for (WindowOverlay *overlay : mOverlays) {
				if (overlay->injectKeyPress(arg))
					return true;
			}
			return false;
		}

		bool TopLevelWindow::injectKeyRelease(const Input::KeyEventArgs & arg)
		{
			for (WindowOverlay *overlay : mOverlays) {
				if (overlay->injectKeyRelease(arg))
					return true;
			}
			return false;
		}

		static bool propagateInput(Widget *w, const Input::PointerEventArgs &arg, const Vector3 &screenSize, bool (Widget::*f)(const Input::PointerEventArgs&))
		{
			if (!w->isVisible())
				return false;
			
			if (!w->containsPoint(arg.position, screenSize))
				return false;
			
			for (Widget *c : w->children())
			{
				if (propagateInput(c, arg, screenSize, f))
					return true;
			}
			return (w->*f)(arg);			
		}

		bool TopLevelWindow::injectPointerPress(const Input::PointerEventArgs & arg)
		{
			for (WindowOverlay *overlay : mOverlays) {
				if (overlay->injectPointerPress(arg))
					return true;
			}

			Vector3 screenSize = getScreenSize();

			for (Widget *w : uniquePtrToPtr(mTopLevelWidgets))
			{
				if (propagateInput(w, arg, screenSize, &Widget::injectPointerPress))
					return true;
			}
			return false;
		}

		bool TopLevelWindow::injectPointerRelease(const Input::PointerEventArgs & arg)
		{
			for (WindowOverlay *overlay : mOverlays) {
				if (overlay->injectPointerRelease(arg))
					return true;
			}

			auto[_, screenSize] = getAvailableScreenSpace();

			for (Widget *w : uniquePtrToPtr(mTopLevelWidgets))
			{
				if (propagateInput(w, arg, screenSize, &Widget::injectPointerRelease))
					return true;
			}
			return false;
		}

		static Widget *getHoveredWidgetUp(const Vector2 &pos, const Vector3 &screenSize, Widget *current)
		{
			if (!current) {
				return nullptr;
			}
			else if (!current->isVisible() || !current->containsPoint(pos, screenSize))
			{
				return getHoveredWidgetUp(pos, screenSize, current->getParent());
			}
			else
			{
				return current;
			}			
		}

		Widget *TopLevelWindow::getHoveredWidgetDown(const Vector2 &pos, const Vector3 &screenSize, Widget *current)
		{
			const auto &widgets = current ? current->children() : uniquePtrToPtr(mTopLevelWidgets);

			for (Widget *w : widgets)
			{
				if (w->isVisible() && w->containsPoint(pos, screenSize))
				{
					return getHoveredWidgetDown(pos, screenSize, w);
				}
			}
			return current;

		}

		Widget *TopLevelWindow::getHoveredWidget(const Vector2 &pos, const Vector3 &screenSize, Widget *current)
		{
			return getHoveredWidgetDown(pos, screenSize, getHoveredWidgetUp(pos, screenSize, current));
		}

		bool TopLevelWindow::injectPointerMove(const Input::PointerEventArgs & arg)
		{
			auto[screenPos, screenSize] = getAvailableScreenSpace();

			for (WindowOverlay *overlay : mOverlays) {
				if (overlay->injectPointerMove(arg))
					return true;
			}


			Widget *hoveredWidget = getHoveredWidget(arg.position - screenPos.xy(), screenSize, mHoveredWidget);

			if (mHoveredWidget != hoveredWidget)
			{

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

		Window::Window * TopLevelWindow::window()
		{
			return mWindow;
		}

		Render::RenderWindow * TopLevelWindow::getRenderer()
		{
			return mRenderWindow.get();
		}

		GUI::TopLevelWindow & TopLevelWindow::getSelf(bool init)
		{
			if (init)
			{
				checkDependency();
			}
			return *this;
		}

		Scene::SceneComponentBase & TopLevelWindow::getSceneComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mGui.getSceneComponent(i, init);
		}

		App::GlobalAPIBase & TopLevelWindow::getGlobalAPIComponent(size_t i, bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mGui.getGlobalAPIComponent(i, init);
		}

		Scene::SceneManager & TopLevelWindow::sceneMgr(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mGui.sceneMgr(init);
		}

		bool TopLevelWindow::init()
		{
			return mUI->callInit();;
		}

		void TopLevelWindow::finalize()
		{
			mUI->callFinalize();
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
			for (Widget *topLevel : uniquePtrToPtr(mTopLevelWidgets))
			{
				topLevel->screenSizeChanged({ static_cast<float>(width), static_cast<float>(height), 1.0f });
			}
		}

		void TopLevelWindow::calculateWindowGeometries()
		{
			for (Widget *topLevel : uniquePtrToPtr(mTopLevelWidgets))
			{
				topLevel->updateGeometry(getScreenSize(), getSize());
			}
		}


		void TopLevelWindow::destroyTopLevel(Widget* w)
		{
			auto it = std::find_if(mTopLevelWidgets.begin(), mTopLevelWidgets.end(), [=](const std::unique_ptr<Widget> &ptr) {return ptr.get() == w; });
			assert(it != mTopLevelWidgets.end());
			mTopLevelWidgets.erase(it);
		}

		void TopLevelWindow::clear()
		{
			mTopLevelWidgets.clear();
		}

		GUISystem& TopLevelWindow::gui()
		{
			return mGui;
		}

		Widget* TopLevelWindow::getWidget(const std::string& name)
		{
			return mWidgets.at(name);
		}

		void TopLevelWindow::registerWidget(Widget* w)
		{
			if (!w->getName().empty()) {
				auto pib = mWidgets.try_emplace(w->getName(), w);
				assert(pib.second);
			}
		}

		void TopLevelWindow::unregisterWidget(Widget* w)
		{
			if (!w->getName().empty()) {
				size_t result = mWidgets.erase(w->getName());
				assert(result == 1);
			}
		}

	}
}
