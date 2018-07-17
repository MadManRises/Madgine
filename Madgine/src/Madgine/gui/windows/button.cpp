#include "../../clientlib.h"

#include "button.h"

namespace Engine
{
	namespace GUI
	{
		void Button::emitClicked()
		{
			mClicked.emit();
		}
	}
}