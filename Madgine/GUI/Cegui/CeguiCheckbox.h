#pragma once

#include "GUI\Windows\Checkbox.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			class CeguiCheckbox : public Checkbox
			{
			public:
				CeguiCheckbox(Window *w);
				~CeguiCheckbox();

				// Inherited via Checkbox
				virtual bool isChecked() override;
				virtual void setChecked(bool b) override;
			};
		}
	}
}


