#include "baselib.h"

#include "MadgineObjectCollector.h"

namespace Engine {
		void MadgineObjectCollector::add(MadgineObjectBase * object)
		{
			mObjects.push_back(object);
		}
		void MadgineObjectCollector::remove(MadgineObjectBase * object)
		{
			mObjects.remove(object);
		}
		std::list<MadgineObjectBase*>::const_iterator MadgineObjectCollector::begin()
		{
			return mObjects.begin();
		}
		std::list<MadgineObjectBase*>::const_iterator MadgineObjectCollector::end()
		{
			return mObjects.end();
		}
		MadgineObjectCollector &MadgineObjectCollector::getSingleton()
		{
			static MadgineObjectCollector singleton;
			return singleton;
		}


}