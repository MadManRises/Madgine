#pragma once

#include "widget.h"

#include "../../signalslot/signal.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT Button : public virtual Widget
		{
		public:
			using Widget::Widget;
			virtual ~Button() = default;
			virtual void setText(const std::string& text) = 0;

			SignalSlot::SignalStub<> &clickEvent();

		protected:
			void emitClicked();

		private:
			SignalSlot::Signal<> mClicked;
		};

	}
}
