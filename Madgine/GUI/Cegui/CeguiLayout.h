#pragma once
#include "GUI\Windows\Layout.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CeguiLayout :
				public Layout
			{
			public:
				CeguiLayout(Window *w);
				~CeguiLayout();

				// Inherited via Layout
				virtual void layout() override;
			};
		}
	}
}


