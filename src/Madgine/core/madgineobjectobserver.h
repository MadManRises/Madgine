#pragma once

#include "Modules/keyvalue/observablecontainer.h"

namespace Engine
{
	namespace Core {

		struct MADGINE_BASE_EXPORT MadgineObjectObserver
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