#pragma once

#include "Interfaces/scripting/types/scope.h"
#include "../../input/inputlistener.h"
#include "Interfaces/window/windoweventlistener.h"
#include "../../input/inputcollector.h"
#include "../../uniquecomponent/uniquecomponentselector.h"

#include "Interfaces/generic/transformIt.h"

namespace Engine
{
	namespace GUI
	{
		
		class MADGINE_CLIENT_EXPORT TopLevelWindow : public Scripting::Scope<TopLevelWindow>, public Input::InputListener, public Window::WindowEventListener
		{

		public:
			TopLevelWindow(GUISystem &gui);
			virtual ~TopLevelWindow();

			void close();

			/*void showCursor();
			void hideCursor();

			virtual bool isCursorVisible();
			virtual void setCursorVisibility(bool v);
			virtual void setCursorPosition(const Vector2& pos);
			virtual Vector2 getCursorPosition();*/
			Vector3 getScreenSize();
			std::pair<Vector3, Vector3> getAvailableScreenSpace();

			Matrix3 getSize();

			Widget *createTopLevelWidget(const std::string &name);
			Bar *createTopLevelBar(const std::string &name);
			Button *createTopLevelButton(const std::string &name);
			Checkbox *createTopLevelCheckbox(const std::string &name);
			Combobox *createTopLevelCombobox(const std::string &name);
			Image *createTopLevelImage(const std::string &name);
			Label *createTopLevelLabel(const std::string &name);
			SceneWindow *createTopLevelSceneWindow(const std::string &name);
			TabWidget *createTopLevelTabWidget(const std::string &name);
			Textbox *createTopLevelTextbox(const std::string &name);
			

			void destroyTopLevel(Widget* w);

			void clear();

			GUISystem &gui();

			KeyValueMapList maps() override;

			Input::InputHandler *input();

			void addOverlay(WindowOverlay *overlay);

			bool injectKeyPress(const Input::KeyEventArgs& arg) override;
			bool injectKeyRelease(const Input::KeyEventArgs& arg) override;
			bool injectMousePress(const Input::MouseEventArgs& arg) override;
			bool injectMouseRelease(const Input::MouseEventArgs& arg) override;
			bool injectMouseMove(const Input::MouseEventArgs& arg) override;

			void renderOverlays();

			Window::Window *window();

			decltype(auto) widgets()
			{
				return uniquePtrToPtr(mTopLevelWidgets);
			}

			Render::RenderWindow *getRenderer();

		protected:			

			void onClose() override;
			void onRepaint() override;
			void onResize(size_t width, size_t height) override;

			void calculateWindowGeometries();

			Widget *getHoveredWidget(const Vector2 &pos, const Vector3 &screenSize, Widget *current);
			Widget *getHoveredWidgetDown(const Vector2 &pos, const Vector3 &screenSize, Widget *current);


			std::unique_ptr<Widget> createWidgetClass(const std::string& name, Class _class);

			virtual std::unique_ptr<Widget> createWidget(const std::string &name);
			virtual std::unique_ptr<Bar> createBar(const std::string& name);
			virtual std::unique_ptr<Button> createButton(const std::string& name);
			virtual std::unique_ptr<Checkbox> createCheckbox(const std::string& name);
			virtual std::unique_ptr<Combobox> createCombobox(const std::string& name);
			virtual std::unique_ptr<Image> createImage(const std::string& name);
			virtual std::unique_ptr<Label> createLabel(const std::string& name);
			virtual std::unique_ptr<SceneWindow> createSceneWindow(const std::string& name);
			virtual std::unique_ptr<TabWidget> createTabWidget(const std::string& name);
			virtual std::unique_ptr<Textbox> createTextbox(const std::string& name);

		private:

			std::vector<std::unique_ptr<Widget>> mTopLevelWidgets;
			GUISystem &mGui;

			Input::InputHandler *mExternalInput = nullptr;
			std::optional<Input::InputSelector> mInputSelector;

			std::vector<WindowOverlay*> mOverlays;

			Window::Window *mWindow = nullptr;
			std::unique_ptr<Render::RenderWindow> mRenderWindow;			

			Widget *mHoveredWidget = nullptr;
		};

	}
}


RegisterClass(Engine::GUI::TopLevelWindow);