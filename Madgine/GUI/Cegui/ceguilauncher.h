#ifndef CEGUILAUNCHER_H
#define CEGUILAUNCHER_H

#include "uniquecomponentcollector.h"
#include "App/contextmasks.h"
#include "GUI\GUISystem.h"

#include <CEGUI\CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CeguiWindow;

			class CEGUILauncher : public GUISystem {
			public:
				CEGUILauncher();
				virtual ~CEGUILauncher();

				///Rendering

				void renderSingleFrame();

				CEGUI::Texture *createTexture(const std::string &name, Ogre::TexturePtr &ogreTex, bool take_ownership);

				///WindowHandling




				Window *loadLayout(const std::string &layout, const std::string &parent, Class _class = WINDOW_CLASS);



				static CEGUI::MouseButton convertButton(Engine::GUI::MouseButton buttonID);
				static Engine::GUI::MouseButton convertButton(CEGUI::MouseButton buttonID);

				static std::function<bool(const CEGUI::EventArgs&)> convert(std::function<void(GUI::MouseEventArgs&)> f);
				static std::function<bool(const CEGUI::EventArgs&)> convert(std::function<bool(GUI::KeyEventArgs&)> f);
				static std::function<bool(const CEGUI::EventArgs&)> convert(std::function<void()> f);

				// Inherited via GUISystem
				virtual void injectKeyPress(const GUI::KeyEventArgs & arg) override;

				virtual void injectKeyRelease(const GUI::KeyEventArgs & arg) override;

				virtual void injectMousePress(const GUI::MouseEventArgs & arg) override;

				virtual void injectMouseRelease(const GUI::MouseEventArgs & arg) override;

				virtual void injectMouseMove(const GUI::MouseEventArgs & arg) override;


				// Inherited via GUISystem
				virtual void injectTimePulse(float time) override;

				virtual void notifyDisplaySizeChanged(const Ogre::Vector2 & size) override;

				virtual bool isCursorVisible() override;

				virtual GUI::Window *getWindow(const std::string & name, GUI::Class _class, GUI::Window * window = 0) override;


				// Inherited via GUISystem
				virtual GUI::Window *createWindow(const std::string & name = "", Class _class = WINDOW_CLASS) override;

				//virtual void destroyWindow(WindowId window) override;


				// Inherited via GUISystem
				virtual void setCursorPosition(const Ogre::Vector2 &pos) override;
				virtual Ogre::Vector2 getCursorPosition() override;
				virtual void setCursorVisibility(bool b) override;


			private:



				bool mKeepingCursorPos;

				CEGUI::Vector2f mKeptCursorPosition;

				CEGUI::BasicImage *mGameImage;
				Ogre::TexturePtr mCurrentTexture;

				CEGUI::Window *mRootWindow;





			private:
				//System pointers
				CEGUI::System *mSystem;
				CEGUI::GUIContext *mContext;
				CEGUI::WindowManager *mWinMgr;
				CEGUI::SchemeManager *mSchemeMgr;
				CEGUI::ImageManager *mImageMgr;



				CEGUI::OgreRenderer *mRenderer;


			private:
				std::map<CEGUI::Window*, Window*> mWindows;
				CEGUI::Window *map(CeguiWindow*);
				Window *map(CEGUI::Window*, Class);



				// Inherited via GUISystem
				virtual Window * map(Window::WindowId w) override;

				virtual bool isMapped(Window::WindowId w) override;

				virtual std::list<Window::WindowId> internalChildren(Window::WindowId w) override;

				virtual Window::WindowId internalRoot() override;

				CEGUI::Window *getWindowByName(const std::string &windowName);
				CEGUI::Window *getActiveWindow();

			};
		}
	}
}

#endif // CEGUILAUNCHER_H
