#pragma once

#include "../../scripting/types/scope.h"
#include "../../input/inputlistener.h"

namespace Engine
{
	namespace GUI
	{
		
		class MADGINE_CLIENT_EXPORT TopLevelWindow : public Scripting::Scope<TopLevelWindow>, public Input::InputListener
		{

		public:
			TopLevelWindow(GUISystem &gui);
			virtual ~TopLevelWindow();

			virtual bool update();

			void showCursor();
			void hideCursor();

			virtual bool isCursorVisible() = 0;
			virtual void setCursorVisibility(bool v) = 0;
			virtual void setCursorPosition(const Vector2& pos) = 0;
			virtual Vector2 getCursorPosition() = 0;
			virtual Vector3 getScreenSize() = 0;



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

			virtual bool singleFrame() = 0;

			KeyValueMapList maps() override;

			Input::InputHandler *input();

		protected:
			void setInput(Input::InputHandler *input);

			void calculateWindowGeometries();


			std::unique_ptr<Widget> createWidgetClass(const std::string& name, Class _class);

			virtual std::unique_ptr<Widget> createWidget(const std::string &name) = 0;
			virtual std::unique_ptr<Bar> createBar(const std::string& name) = 0;
			virtual std::unique_ptr<Button> createButton(const std::string& name) = 0;
			virtual std::unique_ptr<Checkbox> createCheckbox(const std::string& name) = 0;
			virtual std::unique_ptr<Combobox> createCombobox(const std::string& name) = 0;
			virtual std::unique_ptr<Image> createImage(const std::string& name) = 0;
			virtual std::unique_ptr<Label> createLabel(const std::string& name) = 0;
			virtual std::unique_ptr<SceneWindow> createSceneWindow(const std::string& name) = 0;
			virtual std::unique_ptr<TabWidget> createTabWidget(const std::string& name) = 0;
			virtual std::unique_ptr<Textbox> createTextbox(const std::string& name) = 0;

		private:

			std::vector<std::unique_ptr<Widget>> mTopLevelWidgets;
			GUISystem &mGui;

			Input::InputHandler *mInput;
		};

	}
}