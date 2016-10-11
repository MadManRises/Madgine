#pragma once
#include "GUI\Windows\Button.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CeguiButton :
				public Button
			{
			public:
				CeguiButton(Window *w);
				~CeguiButton();

				// Inherited via Button
				virtual void registerOnClickEvent(std::function<void()> f) override;
			};
		}
	}
}


