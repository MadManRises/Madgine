#pragma once

#include "GUI/Windows/BaseWindow.h"

#include "CEGUI\CEGUI.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CEGUILauncher;

			class CeguiWindow : public BaseWindow
			{
			public:
				CeguiWindow(CEGUI::Window *window, CEGUILauncher *cegui);
				~CeguiWindow();

				// Inherited via Window
				virtual void showModal() override;

				// Inherited via Window
				virtual void registerHandlerEvents(std::function<void(GUI::MouseEventArgs&)> mouseMove, std::function<void(GUI::MouseEventArgs&)> mouseDown, std::function<void(GUI::MouseEventArgs&)> mouseUp, std::function<void(GUI::MouseEventArgs&)> mouseScroll, std::function<bool(GUI::KeyEventArgs&)> keyPress) override;


				CEGUI::Window *window();

				virtual void registerEvent(EventType type, std::function<void()> event) override;


				virtual WindowSizeRelVector size() override;
				virtual Ogre::Vector2 pixelSize() override;


				virtual std::string name() override;
				virtual void setName(const std::string & text) override;


				virtual WindowId intern() override;

				// Inherited via Window
				virtual bool isRoot() override;

				virtual Ogre::Vector2 parentSize() override;

				virtual void setPixelSize(const Ogre::Vector2 & size) override;



				// Inherited via Window
				virtual bool isOpen() override;
				virtual void releaseInput() override;
				virtual void captureInput() override;
				virtual void hideModal() override;
				virtual void show() override;
				virtual void hide() override;
				virtual void activate() override;
				virtual void destroy() override;
				virtual void setEnabled(bool b) override;
				virtual void addChild(Window * w) override;
				virtual Window * getChild(const std::string & name, Class _class = WINDOW_CLASS) override;

			private:
				CEGUI::Window *mWindow;

				std::list<CEGUI::Event::Connection> mConnections;

				CEGUILauncher *mCegui;

			};
		}
	}
}



