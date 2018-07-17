#pragma once

#include "Madgine/gui/windows/bar.h"
#include "myguiwindow.h"

namespace Engine
{
	namespace GUI
	{
		
		class MyGUIBar : public Bar, public MyGUIWindow
		{
		public:
			MyGUIBar(const std::string &name, MyGUIWindow *parent);
			MyGUIBar(const std::string &name, MyGUISystem &system);

			void setRatio(float f) override;
			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;
			
		private:
			MyGUI::ProgressBar *mBar;
		};

	}
}