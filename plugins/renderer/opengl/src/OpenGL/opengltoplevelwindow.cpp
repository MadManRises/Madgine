#include "opengllib.h"

#include "opengltoplevelwindow.h"
#include "openglsystem.h"

#include "Madgine/gui/widgets/bar.h"
#include "Madgine/gui/widgets/button.h"
#include "Madgine/gui/widgets/checkbox.h"
#include "Madgine/gui/widgets/combobox.h"
#include "Madgine/gui/widgets/image.h"
#include "Madgine/gui/widgets/label.h"
#include "Madgine/gui/widgets/scenewindow.h"
#include "Madgine/gui/widgets/tabwidget.h"
#include "Madgine/gui/widgets/textbox.h"


#include "Interfaces/window/windowapi.h"

#include "glad.h"

namespace Engine {
	namespace GUI {

		OpenGLTopLevelWindow::OpenGLTopLevelWindow(OpenGLSystem & system) :
			TopLevelWindow(system)
		{			

			Engine::Window::WindowSettings settings;
			Window::Window *tmp = Window::sCreateWindow(settings);

			PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
				PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
				32,                   // Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24,                   // Number of bits for the depthbuffer
				8,                    // Number of bits for the stencilbuffer
				0,                    // Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};

			HDC ourWindowHandleToDeviceContext = GetDC((HWND)tmp->mHandle);

			int  letWindowsChooseThisPixelFormat;
			letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd);
			SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd);

			HGLRC ourOpenGLRenderingContext = wglCreateContext(ourWindowHandleToDeviceContext);
			wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);

			typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
			PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(ourOpenGLRenderingContext);
			ReleaseDC((HWND)tmp->mHandle, ourWindowHandleToDeviceContext);
			tmp->destroy();

			ourWindowHandleToDeviceContext = GetDC((HWND)window()->mHandle);

			letWindowsChooseThisPixelFormat = ChoosePixelFormat(ourWindowHandleToDeviceContext, &pfd);
			SetPixelFormat(ourWindowHandleToDeviceContext, letWindowsChooseThisPixelFormat, &pfd);
			
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

			int attribs[] = { 
				WGL_CONTEXT_PROFILE_MASK_ARB, GL_TRUE,
				0 
			};

			ourOpenGLRenderingContext = wglCreateContextAttribsARB(ourWindowHandleToDeviceContext, NULL, attribs);
			wglMakeCurrent(ourWindowHandleToDeviceContext, ourOpenGLRenderingContext);

			typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

			wglSwapIntervalEXT(0);

			gladLoadGL();		

			//glViewport(0, 0, window()->width(), window()->height());

			/*Widget *loading = createTopLevelImage("Loading");
			Widget *progress = loading->createChildBar("ProgressBar");
			progress->setSize({ 0.8f,0,0,0,0.1f,0,0,0,1 });
			progress->setPos({ 0.1f,0,0,0,0.85f,0,0,0,0 });
			Widget *loadMsg = loading->createChildLabel("LoadingMsg");
			Widget *ingame = createTopLevelWidget("ingame");
			Widget *game = ingame->createChildSceneWindow("game");
			game->setSize({ 0.8f,0,0,0,1,0,0,0,1 });
			Widget *placeBaseButton = game->createChildButton("PlaceBaseButton");
			placeBaseButton->setPos({ 1,0,-120,0,0,30,0,0,0 });
			placeBaseButton->setSize({ 0,0,100,0,0,30,0,0,1 });
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
			Widget *lobbyList = lobbyListMenu->createChildCombobox("LobbyList");*/


		}

		bool OpenGLTopLevelWindow::update()
		{
			if (!TopLevelWindow::update())
				return false;

			window()->beginFrame();
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			renderOverlays();
			window()->endFrame();

			return true;
		}
		bool OpenGLTopLevelWindow::handleKeyPress(const Input::KeyEventArgs & arg)
		{
			return true;
		}
		bool OpenGLTopLevelWindow::handleKeyRelease(const Input::KeyEventArgs & arg)
		{
			return true;
		}
		bool OpenGLTopLevelWindow::handleMousePress(const Input::MouseEventArgs & arg)
		{
			return true;
		}
		bool OpenGLTopLevelWindow::handleMouseRelease(const Input::MouseEventArgs & arg)
		{
			return true;
		}
		bool OpenGLTopLevelWindow::handleMouseMove(const Input::MouseEventArgs & arg)
		{
			return true;
		}
		bool OpenGLTopLevelWindow::isCursorVisible()
		{
			return false;
		}
		void OpenGLTopLevelWindow::setCursorVisibility(bool v)
		{
		}
		void OpenGLTopLevelWindow::setCursorPosition(const Vector2 & pos)
		{
		}
		Vector2 OpenGLTopLevelWindow::getCursorPosition()
		{
			return Vector2();
		}
		Vector3 OpenGLTopLevelWindow::getScreenSize()
		{
			return { static_cast<float>(window()->renderWidth()), static_cast<float>(window()->renderHeight()), 1 };
		}
		bool OpenGLTopLevelWindow::singleFrame()
		{
			return false;
		}
		std::unique_ptr<Widget> OpenGLTopLevelWindow::createWidget(const std::string & name)
		{
			return std::unique_ptr<Widget>();
		}
		std::unique_ptr<Bar> OpenGLTopLevelWindow::createBar(const std::string & name)
		{
			return std::unique_ptr<Bar>();
		}
		std::unique_ptr<Button> OpenGLTopLevelWindow::createButton(const std::string & name)
		{
			return std::unique_ptr<Button>();
		}
		std::unique_ptr<Checkbox> OpenGLTopLevelWindow::createCheckbox(const std::string & name)
		{
			return std::unique_ptr<Checkbox>();
		}
		std::unique_ptr<Combobox> OpenGLTopLevelWindow::createCombobox(const std::string & name)
		{
			return std::unique_ptr<Combobox>();
		}
		std::unique_ptr<Image> OpenGLTopLevelWindow::createImage(const std::string & name)
		{
			return std::unique_ptr<Image>();
		}
		std::unique_ptr<Label> OpenGLTopLevelWindow::createLabel(const std::string & name)
		{
			return std::unique_ptr<Label>();
		}
		std::unique_ptr<SceneWindow> OpenGLTopLevelWindow::createSceneWindow(const std::string & name)
		{
			return std::unique_ptr<SceneWindow>();
		}
		std::unique_ptr<TabWidget> OpenGLTopLevelWindow::createTabWidget(const std::string & name)
		{
			return std::unique_ptr<TabWidget>();
		}
		std::unique_ptr<Textbox> OpenGLTopLevelWindow::createTextbox(const std::string & name)
		{
			return std::unique_ptr<Textbox>();
		}
	}
}