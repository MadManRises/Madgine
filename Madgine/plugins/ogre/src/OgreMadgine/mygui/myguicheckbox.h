#pragma once

#include "Madgine/gui/widgets/checkbox.h"
#include "myguiwidget.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUICheckbox : public Checkbox, public MyGUIWidget
		{
		public:
			MyGUICheckbox(const std::string &name, MyGUIWidget *parent);
			MyGUICheckbox(const std::string &name, MyGUITopLevelWindow &system);

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