#pragma once

#include "Interfaces/generic/observablecontainer.h"

namespace Engine
{
	namespace Core {

		struct MadgineObjectObserver
		{

			void handle(MadgineObject *object, int event) const;

			template <typename It>
			void operator()(const It &it, int event) const
			{
				handle(&**it, event);
			}

		};
	}
}