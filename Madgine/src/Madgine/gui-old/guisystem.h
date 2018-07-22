#pragma once

#include "guievents.h"
#include "windows/windowclass.h"
#include "../madgineobject.h"
#include "../scripting/types/scopebase.h"
#include "../scene/scenecomponentbase.h"
#include "../scripting/types/globalapicomponentbase.h"
#include "../core/frameloop.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT GUISystem :
			public MadgineObject,
			public Ogre::GeneralAllocatedObject,
			public Ogre::WindowEventListener,
			public Scripting::ScopeBase,
			public Core::FrameLoop
		{
		public:
			GUISystem(App::ClientApplication &app);
			virtual ~GUISystem();

			//void printHierarchy();

			//virtual void update(float time);
			virtual void notifyDisplaySizeChanged(const Ogre::Vector2& size);

			virtual bool preInit() override;
			virtual bool init() override;
			virtual void finalize() override;

			void showCursor();
			void hideCursor();

			virtual int go() override = 0;

			virtual void injectKeyPress(const KeyEventArgs& arg) = 0;
			virtual void injectKeyRelease(const KeyEventArgs& arg) = 0;
			virtual void injectMousePress(const MouseEventArgs& arg) = 0;
			virtual void injectMouseRelease(const MouseEventArgs& arg) = 0;
			virtual void injectMouseMove(const MouseEventArgs& arg) = 0;

			virtual bool isCursorVisible() = 0;
			virtual void setCursorVisibility(bool v) = 0;
			virtual void setCursorPosition(const Ogre::Vector2& pos) = 0;
			virtual Ogre::Vector2 getCursorPosition() = 0;
			virtual Ogre::Vector2 getScreenSize() = 0;


			void addWindow(const std::string& name, WindowContainer* w);
			void removeWindow(const std::string& name);

			void updateWindowSizes();
			void setDirtyWindowSizes();

			Window* getWindowByName(const std::string& name, Class _class = Class::WINDOW_CLASS);
			Window* loadLayout(const std::string& name, const std::string& parent);

			Window* getRootWindow() const;

			/**
			* Sets the properties of the Renderwindow. Might have unexpected effects, when used as embedded window.
			*
			* @param fullscreen flag indicating, if the window should be shown in fullscreen
			* @param width the preferred width for the Renderwindow
			* @param height the preferred height of the Renderwindow
			*/
			virtual void setWindowProperties(bool fullscreen, unsigned int width, unsigned int height) = 0;
			void windowResized(Ogre::RenderWindow* rw) override;

			/**
			* For embedded Applications. Resizes the Game-Components to the current size of the Renderwindow. Will be called automatically in a non-embedded environment.
			*/
			virtual void resizeWindow() = 0;

			const char* key() const;

			App::ClientApplication &app();

			Scene::SceneComponentBase &getSceneComponent(size_t i);

			Scripting::GlobalAPIComponentBase &getGlobalAPIComponent(size_t i);

			Scene::SceneManager &sceneMgr();
			UI::UIManager &ui();


		protected:
			WindowContainer* mRootWindow;

		private:
			bool mWindowSizesDirty;

			std::map<std::string, WindowContainer *> mWindows;

			App::ClientApplication &mApp;

			std::unique_ptr<UI::UIManager> mUI;

		};
	}
}
