#pragma once

#include "guievents.h"
#include "windows/windowclass.h"
#include "madgineobject.h"

namespace Engine {
	namespace GUI {

		

		class OGREMADGINE_EXPORT GUISystem : 
			public Singleton<GUISystem>, 
			public MadgineObject,
			public Ogre::GeneralAllocatedObject,
			public Ogre::WindowEventListener,
			public Scripting::ScopeBase
		{
		public:
			GUISystem();
			virtual ~GUISystem();

			void printHierarchy();

			virtual void update(float time);
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
			void setDirtyWindowSizes();

			Window *getWindowByName(const std::string &name, Class _class = Class::WINDOW_CLASS);
			Window *loadLayout(const std::string &name, const std::string &parent);

			Window *getRootWindow();

			virtual void windowResized(Ogre::RenderWindow * rw) override;

			const char *key() const;

		protected:
			WindowContainer *mRootWindow;

		private:
			bool mWindowSizesDirty;

			std::map<std::string, WindowContainer *> mWindows;
			
		};
	}
}



