#pragma once
#include "GUI\Windows\Label.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CeguiLabel :
				public Label
			{
			public:
				CeguiLabel(Window *w);
				~CeguiLabel();

				// Inherited via Label
				virtual void setText(const std::string & text) override;
			};
		}
	}
}
