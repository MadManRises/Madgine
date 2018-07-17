#pragma once

#include "handler.h"

namespace Engine
{
	namespace UI
	{
		
		template <class T = GUI::Window>
		class WindowPtr
		{
		public:
			WindowPtr(Handler *handler, const std::string &name) :
				mPtr(nullptr)
			{
				handler->registerWindow(name, [this](GUI::Window *w)
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