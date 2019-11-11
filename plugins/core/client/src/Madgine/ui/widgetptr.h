#pragma once

#include "handler.h"

namespace Engine
{
	namespace UI
	{
		
		template <class T = Widgets::WidgetBase>
		class WidgetPtr
		{
		public:
			WidgetPtr(Handler *handler, const std::string &name) :
				mPtr(nullptr)
			{
                            handler->registerWidget(name, [this](Widgets::WidgetBase *w)
				{
					mPtr = dynamic_cast<T*>(w);
					return mPtr;
				});
			}

			operator T*() const {
				return mPtr;
			}

			T *operator->() const
			{
				return mPtr;
			}

			T &operator* () const{
				return *mPtr;
			}

		private:
			T * mPtr;
		};

	}
}