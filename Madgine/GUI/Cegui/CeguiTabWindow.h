#pragma once

#include "GUI/Windows/TabWindow.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CeguiTabWindow : public TabWindow
			{
			public:
				CeguiTabWindow(Window * w);
				~CeguiTabWindow();

				// Inherited via TabWindow
				virtual void addTab(Window * w) override;
			};
		}
	}
}
