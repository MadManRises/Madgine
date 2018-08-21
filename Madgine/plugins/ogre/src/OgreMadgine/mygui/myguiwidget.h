#pragma once

#include "Madgine/gui/widgets/widget.h"

namespace Engine
{
	namespace GUI
	{
		

		class MyGUIWidget : public virtual Widget
		{
		public:
			MyGUIWidget(const std::string& name, MyGUIWidget* parent);

			MyGUIWidget(const std::string& name, MyGUITopLevelWindow& window);


			Vector3 getAbsoluteSize() const override;
			Vector2 getAbsolutePosition() const override;			
			void releaseInput() override;
			void captureInput() override;
			void activate() override;
			void moveToFront() override;
			bool isVisible() const override;
			void showModal() override;
			void hideModal() override;
			void setVisible(bool b) override;
			void setEnabled(bool b) override;

			virtual MyGUI::Widget *widget() const = 0;

		protected:

			std::unique_ptr<Widget> createWidget(const std::string& name) override;
			std::unique_ptr<Bar> createBar(const std::string& name) override;
			std::unique_ptr<Button> createButton(const std::string& name) override;
			std::unique_ptr<Checkbox> createCheckbox(const std::string& name) override;
			std::unique_ptr<Combobox> createCombobox(const std::string& name) override;
			std::unique_ptr<Image> createImage(const std::string& name) override;
			std::unique_ptr<Label> createLabel(const std::string& name) override;
			std::unique_ptr<SceneWindow> createSceneWindow(const std::string& name) override;
			std::unique_ptr<TabWidget> createTabWidget(const std::string& name) override;
			std::unique_ptr<Textbox> createTextbox(const std::string& name) override;
		
			void setAbsoluteSize(const Vector3& size) override;
			void setAbsolutePosition(const Vector2& pos) override;
			

			void setup();

			void emitMouseMove(MyGUI::Widget *w, int left, int top);
			void emitMouseWheel(MyGUI::Widget *w, int wheel);
			void emitMouseDown(MyGUI::Widget *w, int left, int top, MyGUI::MouseButton button);
			void emitMouseUp(MyGUI::Widget *w, int left, int top, MyGUI::MouseButton button);

		private:
			float mScale;

		};

	}
}