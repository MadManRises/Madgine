#include "libinclude.h"

#include "CeguiCheckbox.h"

#include <CEGUI\CEGUI.h>

namespace Engine {
	namespace GUI {
		namespace Cegui {
			CeguiCheckbox::CeguiCheckbox(Window *w) :
				Checkbox(w)
			{
			}


			CeguiCheckbox::~CeguiCheckbox()
			{
			}

			bool CeguiCheckbox::isChecked()
			{
				return getIntern<CEGUI::ToggleButton>()->isSelected();
			}

			void CeguiCheckbox::setChecked(bool b)
			{
				getIntern<CEGUI::ToggleButton>()->setSelected(b);
			}
		}
	}
}