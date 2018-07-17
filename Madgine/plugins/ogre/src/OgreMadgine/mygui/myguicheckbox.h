#pragma once

#include "Madgine/gui/windows/checkbox.h"
#include "myguiwindow.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUICheckbox : public Checkbox, public MyGUIWindow
		{
		public:
			MyGUICheckbox(const std::string &name, MyGUIWindow *parent);
			MyGUICheckbox(const std::string &name, MyGUISystem &system);

			bool isChecked() override;
			void setChecked(bool b) override;

			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;

		private:
			void toggle(MyGUI::Widget* t);

			MyGUI::Button *mCheckbox;
		};

	}
}