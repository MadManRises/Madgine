#pragma once

#include "Madgine/gui/windows/window.h"

namespace Engine
{
	namespace GUI
	{
		

		class MyGUIWindow : public virtual Window
		{
		public:
			MyGUIWindow(const std::string& name, MyGUIWindow* parent);

			MyGUIWindow(const std::string& name, MyGUISystem& gui);


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

			std::unique_ptr<Window> createWindow(const std::string& name) override;
			std::unique_ptr<Bar> createBar(const std::string& name) override;
			std::unique_ptr<Button> createButton(const std::string& name) override;
			std::unique_ptr<Checkbox> createCheckbox(const std::string& name) override;
			std::unique_ptr<Combobox> createCombobox(const std::string& name) override;
			std::unique_ptr<Image> createImage(const std::string& name) override;
			std::unique_ptr<Label> createLabel(const std::string& name) override;
			std::unique_ptr<SceneWindow> createSceneWindow(const std::string& name) override;
			std::unique_ptr<TabWindow> createTabWindow(const std::string& name) override;
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