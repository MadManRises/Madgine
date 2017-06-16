#pragma once

#include "Serialize\serializable.h"
#include "Serialize\Container\unithelper.h"
#include "uniquecomponentcollector.h"

namespace Engine{

	template <class Base>
	class OGREMADGINE_EXPORT OgreCreatorStore {
	protected:
		static std::list<std::function<std::unique_ptr<Base>()>> &sComponents() {
			static std::list<std::function<std::unique_ptr<Base>()>> dummy;
			return dummy;
		}

	};

	template <class Base>
	class OGREMADGINE_EXPORT OgreUniqueComponentCollector : public UniqueComponentCollector<Base, OgreCreatorStore<Base>>, public Singleton<OgreUniqueComponentCollector<Base>> {
		
	};

}
