#include "../baselib.h"

#include "madgineobjectobserver.h"

#include "madgineobject.h"
#include "../app/application.h"

#include "Modules/threading/frameloop.h"

namespace Engine {

	namespace Core {

		void MadgineObjectObserver::handle(MadgineObject * object, int event) const
		{
			switch (event)
			{
			case INSERT_ITEM:
				if (!object->parent() || object->parent()->isInitialized())
				{
					object->app(false).frameLoop().queue([=]() {
						object->callInit(); 
					});
				}
				break;
			case BEFORE | REMOVE_ITEM:
				object->callFinalize();
				break;
			}
		}

	}

}