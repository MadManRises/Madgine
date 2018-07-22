#include "../../clientlib.h"

#include "button.h"

namespace Engine
{
	namespace GUI
	{
		SignalSlot::SignalStub<>& Button::clickEvent()
		{
			return mClicked;
		}

		void Button::emitClicked()
		{
			mClicked.emit();
		}
	}
}