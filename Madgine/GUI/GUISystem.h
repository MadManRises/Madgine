#pragma once

#include "GUIEvents.h"
#include "Windows\windowclass.h"

namespace Engine {
	namespace GUI {

		

		class MADGINE_EXPORT GUISystem : public Ogre::Singleton<GUISystem>, public Ogre::GeneralAllocatedObject
		{
		public:
			GUISystem();
			virtual ~GUISystem();

			void printHierarchy();

			virtual void init();
			virtual void injectTimePulse(float time);
			virtual void notifyDisplaySizeChanged(const Ogre::Vector2 &size);
			virtual void renderSingleFrame();

			void showCursor();
			void hideCursor();

			virtual void injectKeyPress(const KeyEventArgs &arg) = 0;
			virtual void injectKeyRelease(const KeyEventArgs &arg) = 0;
			virtual void injectMousePress(const MouseEventArgs &arg) = 0;
			virtual void injectMouseRelease(const MouseEventArgs &arg) = 0;
			virtual void injectMouseMove(const MouseEventArgs &arg) = 0;

			virtual bool isCursorVisible() = 0;
			virtual void setCursorVisibility(bool v) = 0;
			virtual void setCursorPosition(const Ogre::Vector2 &pos) = 0;
			virtual Ogre::Vector2 getCursorPosition() = 0;
			virtual Ogre::Vector2 getScreenSize() = 0;


			void addWindow(const std::string &name, WindowContainer *w);
			void removeWindow(const std::string &name);

			void updateWindowSizes();

			Window *getWindowByName(const std::string &name, Class _class = Class::WINDOW_CLASS);
			Window *loadLayout(const std::string &name, const std::string &parent);

			Window *getRootWindow();

		protected:
			WindowContainer *mRootWindow;

		private:

			std::map<std::string, WindowContainer *> mWindows;
			
		};
	}
}



