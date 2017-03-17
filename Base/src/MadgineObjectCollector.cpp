#include "baselib.h"

#include "MadgineObjectCollector.h"

namespace Engine {
		void MadgineObjectCollector::add(MadgineObject * object)
		{
			mObjects.push_back(object);
		}
		void MadgineObjectCollector::remove(MadgineObject * object)
		{
			mObjects.remove(object);
		}
		std::list<MadgineObject*>::const_iterator MadgineObjectCollector::begin()
		{
			return mObjects.begin();
		}
		std::list<MadgineObject*>::const_iterator MadgineObjectCollector::end()
		{
			return mObjects.end();
		}
		MadgineObjectCollector &MadgineObjectCollector::getSingleton()
		{
			static MadgineObjectCollector singleton;
			return singleton;
		}


}