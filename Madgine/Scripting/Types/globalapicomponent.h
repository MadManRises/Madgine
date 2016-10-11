#pragma once

#include "uniquecomponent.h"
#include "globalapi.h"
#include "baseglobalapicomponent.h"

namespace Engine {
	namespace Scripting {

		template <class T>
		class GlobalAPIComponent : public UniqueComponent<T, BaseGlobalAPIComponent>, public GlobalAPI<T> {
		public:
			using UniqueComponent::UniqueComponent;

		};


	}
}