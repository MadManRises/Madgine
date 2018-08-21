#pragma once

#include "Madgine/gui/widgets/combobox.h"
#include "myguiwidget.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUICombobox : public Combobox, public MyGUIWidget
		{
		public:
			MyGUICombobox(const std::string &name, MyGUIWidget *parent);
			MyGUICombobox(const std::string &name, MyGUITopLevelWindow &system);

			void addItem(const std::string& text) override;
			void clear() override;
			void setText(const std::string& s) override;
			std::string getText() override;

			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;

		private:
			MyGUI::ComboBox *mCombobox;
		};

	}
}