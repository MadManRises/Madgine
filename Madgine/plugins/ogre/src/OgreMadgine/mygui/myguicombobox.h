#pragma once

#include "Madgine/gui/windows/combobox.h"
#include "myguiwindow.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUICombobox : public Combobox, public MyGUIWindow
		{
		public:
			MyGUICombobox(const std::string &name, MyGUIWindow *parent);
			MyGUICombobox(const std::string &name, MyGUISystem &system);

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