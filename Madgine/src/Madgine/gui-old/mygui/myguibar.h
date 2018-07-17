#pragma once
#include "../windows/bar.h"


namespace MyGUI
{
	class ProgressBar;
}

namespace Engine
{
	namespace GUI
	{
		namespace MyGui
		{
			class MyGUIWindow;


			class MyGUIBar :
				public Bar
			{
			public:
				MyGUIBar(MyGUIWindow* w);

				// Inherited via Bar
				void setRatio(float f) override;

			private:
				MyGUI::ProgressBar* mBar;
			};
		}
	}
}
