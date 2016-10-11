#include "libinclude.h"
#include "CeguiCombobox.h"

#include "CEGUI\CEGUI.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			CeguiCombobox::CeguiCombobox(Window * w) :
				Combobox(w)
			{
			}

			CeguiCombobox::~CeguiCombobox()
			{
			}

			void CeguiCombobox::addItem(const std::string & text)
			{
				CEGUI::ListboxTextItem *itemCombobox = new CEGUI::ListboxTextItem(text);
				itemCombobox->setSelectionBrushImage("TaharezLook/MultiListSelectionBrush");
				getIntern<CEGUI::Combobox>()->addItem(itemCombobox);
			}

			void CeguiCombobox::clear()
			{
				getIntern<CEGUI::Combobox>()->resetList();
			}

			void CeguiCombobox::setText(const std::string & s)
			{
				getIntern<CEGUI::Combobox>()->setText(s);
			}

			std::string CeguiCombobox::getText()
			{
				return getIntern<CEGUI::Combobox>()->getText().c_str();
			}
		}
	}
}