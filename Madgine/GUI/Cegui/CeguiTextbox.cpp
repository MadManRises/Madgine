#include "libinclude.h"

#include "CeguiTextbox.h"

#include "CEGUI\CEGUI.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			CeguiTextbox::CeguiTextbox(Window * w) :
				Textbox(w)
			{
			}
			std::string CeguiTextbox::getText()
			{
				return getIntern<CEGUI::Editbox>()->getText().c_str();
			}
			void CeguiTextbox::setText(const std::string & text)
			{
				getIntern<CEGUI::Editbox>()->setText(text);
			}
			void CeguiTextbox::setEditable(bool b)
			{
				getIntern<CEGUI::Editbox>()->setReadOnly(!b);
			}
		}
	}
}