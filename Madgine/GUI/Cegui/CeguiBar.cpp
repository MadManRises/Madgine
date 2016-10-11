#include "libinclude.h"

#include "CeguiBar.h"

#include <CEGUI\CEGUI.h>

namespace Engine {
	namespace GUI {
		namespace Cegui {
			CeguiBar::CeguiBar(Window * w) :
				Bar(w)
			{
			}
			void CeguiBar::setRatio(float f)
			{
				getIntern<CEGUI::ProgressBar>()->setProgress(f);
			}
		}
	}
}