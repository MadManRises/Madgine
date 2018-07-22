#pragma once

#include "guievents.h"
#include "windows/windowclass.h"
#include "../madgineobject.h"
#include "../scripting/types/scope.h"
#include "../scene/scenecomponentbase.h"
#include "../core/frameloop.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT GUISystem :			
			public Scripting::Scope<GUISystem>,
			public Serialize::SerializableUnit<GUISystem>,
			public Core::FrameLoop
		{
		public:
			GUISystem(App::ClientApplication &app);
			GUISystem(const GUISystem &) = delete;
			virtual ~GUISystem();

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
			virtual void setCursorPosition(const Vector2& pos) = 0;
			virtual Vector2 getCursorPosition() = 0;
			virtual Vector3 getScreenSize() = 0;
			

			Window* getWindowByName(const std::string& name);

			App::ClientApplication &app(bool = true);

			Scene::SceneComponentBase &getSceneComponent(size_t i, bool = true);

			Scripting::GlobalAPIComponentBase &getGlobalAPIComponent(size_t i, bool = true);

			Scene::SceneManager &sceneMgr(bool = true);
			UI::UIManager &ui(bool = true);

			GUISystem &getSelf(bool = true);

			Window *createTopLevelWindow(const std::string &name);
			Bar *createTopLevelBar(const std::string &name);
			Button *createTopLevelButton(const std::string &name);
			Checkbox *createTopLevelCheckbox(const std::string &name);
			Combobox *createTopLevelCombobox(const std::string &name);
			Image *createTopLevelImage(const std::string &name);
			Label *createTopLevelLabel(const std::string &name);
			SceneWindow *createTopLevelSceneWindow(const std::string &name);
			TabWindow *createTopLevelTabWindow(const std::string &name);
			Textbox *createTopLevelTextbox(const std::string &name);
			

			void registerWindow(Window* w);

			void unregisterWindow(Window *w);

			void destroyTopLevel(Window* w);
			void clear();

			KeyValueMapList maps() override;

		protected:

			virtual bool init() override;
			virtual void finalize() override;

			bool sendFrameRenderingQueued(float timeSinceLastFrame);

			std::unique_ptr<Window> createWindowClass(const std::string& name, Class _class);

			virtual std::unique_ptr<Window> createWindow(const std::string &name) = 0;
			virtual std::unique_ptr<Bar> createBar(const std::string& name) = 0;
			virtual std::unique_ptr<Button> createButton(const std::string& name) = 0;
			virtual std::unique_ptr<Checkbox> createCheckbox(const std::string& name) = 0;
			virtual std::unique_ptr<Combobox> createCombobox(const std::string& name) = 0;
			virtual std::unique_ptr<Image> createImage(const std::string& name) = 0;
			virtual std::unique_ptr<Label> createLabel(const std::string& name) = 0;
			virtual std::unique_ptr<SceneWindow> createSceneWindow(const std::string& name) = 0;
			virtual std::unique_ptr<TabWindow> createTabWindow(const std::string& name) = 0;
			virtual std::unique_ptr<Textbox> createTextbox(const std::string& name) = 0;
			
			void calculateWindowGeometries();

		private:

			std::map<std::string, Window *> mWindows;
			std::vector<std::unique_ptr<Window>> mTopLevelWindows;

			App::ClientApplication &mApp;

			std::unique_ptr<UI::UIManager> mUI;

		};
	}
}
