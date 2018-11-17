#pragma once

#include "Madgine/gui/widgets/bar.h"
#include "myguiwidget.h"

namespace Engine
{
	namespace GUI
	{
		
		class MyGUIBar : public Bar, public MyGUIWidget
		{
		public:
			MyGUIBar(const std::string &name, MyGUIWidget *parent);
			MyGUIBar(const std::string &name, MyGUITopLevelWindow &window);

			void setRatio(float f) override;
			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;
			
		private:
			MyGUI::ProgressBar *mBar;
		};

	}
}