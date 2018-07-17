#pragma once

#include "Madgine/gui/windows/image.h"
#include "myguiwindow.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUIImage : public Image, public MyGUIWindow
		{
		public:
			MyGUIImage(const std::string &name, MyGUIWindow *parent);
			MyGUIImage(const std::string &name, MyGUISystem &system);

			void setImage(const std::string& name) override;

			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;

		private:
			MyGUI::StaticImage *mImage;
		};

	}
}