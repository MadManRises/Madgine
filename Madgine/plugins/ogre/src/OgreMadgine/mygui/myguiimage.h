#pragma once

#include "Madgine/gui/widgets/image.h"
#include "myguiwidget.h"

namespace Engine
{
	namespace GUI
	{

		class MyGUIImage : public Image, public MyGUIWidget
		{
		public:
			MyGUIImage(const std::string &name, MyGUIWidget *parent);
			MyGUIImage(const std::string &name, MyGUITopLevelWindow &system);

			void setImage(const std::string& name) override;
			std::string getImage() const override;

			Class getClass() override;

		protected:
			MyGUI::Widget* widget() const override;

		private:
			MyGUI::StaticImage *mImage;
		};

	}
}