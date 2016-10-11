#pragma once

#include "GUI/Windows/Textbox.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CeguiTextbox : public Textbox
			{
			public:
				CeguiTextbox(Window *w);

				// Inherited via Textbox
				virtual std::string getText() override;
				virtual void setText(const std::string & text) override;
				virtual void setEditable(bool b) override;
			};
		}
	}
}


