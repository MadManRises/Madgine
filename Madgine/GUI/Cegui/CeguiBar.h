#pragma once
#include "GUI\Windows\Bar.h"


namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CeguiBar :
				public Bar
			{
			public:
				CeguiBar(Window *w);

				// Inherited via Bar
				virtual void setRatio(float f) override;
			};
		}
	}
}


