#include "../moduleslib.h"

#include "madgineobjectobserver.h"

#include "madgineobject.h"

#include "../keyvalue/observerevent.h"

namespace Engine {

		void MadgineObjectObserver::handle(MadgineObject * object, int event) const
		{
			switch (event)
			{
			case INSERT_ITEM:
				if (!object->parent() || object->parent()->isInitialized())
				{
					//object->app(false).frameLoop().queue([=]() {
						object->callInit(); 
					//});
				}
				break;
			case BEFORE | REMOVE_ITEM:
				object->callFinalize();
				break;
			}
		}

}