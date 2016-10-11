#include "libinclude.h"

#include "CeguiLayout.h"

#include "CEGUI\CEGUI.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			CeguiLayout::CeguiLayout(Window *w) :
				Layout(w)
			{
			}


			CeguiLayout::~CeguiLayout()
			{
			}

			void CeguiLayout::layout()
			{
				getIntern<CEGUI::LayoutContainer>()->layout();
			}
		}
	}
}