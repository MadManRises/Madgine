#include "libinclude.h"

#include "CeguiLabel.h"

#include "CEGUI\CEGUI.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			CeguiLabel::CeguiLabel(Window *w) :
				Label(w)
			{
			}


			CeguiLabel::~CeguiLabel()
			{
			}

			void CeguiLabel::setText(const std::string & text)
			{
				getIntern<CEGUI::Window>()->setText(text);
			}
		}
	}
}
