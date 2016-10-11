#include "libinclude.h"

#include "CeguiButton.h"

namespace Engine {
	namespace GUI {
		namespace Cegui {
			CeguiButton::CeguiButton(Window *w) :
				Button(w)
			{
			}


			CeguiButton::~CeguiButton()
			{
			}

			void CeguiButton::registerOnClickEvent(std::function<void()> f)
			{
				registerEvent(ButtonClick, f);
			}
		}
	}
}