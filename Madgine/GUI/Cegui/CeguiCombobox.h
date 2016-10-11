#pragma once
#include "GUI\Windows\Combobox.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CeguiCombobox :
				public Combobox
			{
			public:
				CeguiCombobox(Window *w);
				~CeguiCombobox();

				// Inherited via Combobox
				virtual void addItem(const std::string & text) override;
				virtual void clear() override;
				virtual void setText(const std::string & s) override;
				virtual std::string getText() override;
			};
		}
	}
}
