#pragma once

namespace Engine
{

		struct MODULES_EXPORT MadgineObjectObserver
		{

			void handle(MadgineObject *object, int event) const;

			template <typename It>
			void operator()(const It &it, int event) const
			{
				handle(&**it, event);
			}

		};
}