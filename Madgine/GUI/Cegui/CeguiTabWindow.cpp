#include "libinclude.h"

#include "CeguiTabWindow.h"

#include <CEGUI\CEGUI.h>

namespace Engine {
	namespace GUI {
		namespace Cegui {
			CeguiTabWindow::CeguiTabWindow(Window * w) :
				TabWindow(w)
			{
			}


			CeguiTabWindow::~CeguiTabWindow()
			{
			}

			void CeguiTabWindow::addTab(Window * w)
			{
				getIntern<CEGUI::TabControl>()->addTab(w->getIntern<CEGUI::Window>());
			}
		}
	}
}