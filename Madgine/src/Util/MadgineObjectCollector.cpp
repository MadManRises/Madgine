#include "madginelib.h"

#include "MadgineObjectCollector.h"

namespace Engine {
	namespace Util {
		void MadgineObjectCollector::add(BaseMadgineObject * object)
		{
			mObjects.push_back(object);
		}
		void MadgineObjectCollector::remove(BaseMadgineObject * object)
		{
			mObjects.remove(object);
		}
		std::list<BaseMadgineObject*>::const_iterator MadgineObjectCollector::begin()
		{
			return mObjects.begin();
		}
		std::list<BaseMadgineObject*>::const_iterator MadgineObjectCollector::end()
		{
			return mObjects.end();
		}
		MadgineObjectCollector &MadgineObjectCollector::getSingleton()
		{
			static MadgineObjectCollector singleton;
			return singleton;
		}

	}
}