#pragma once

#include "uniquecomponent.h"
#include "globalapi.h"
#include "GlobalAPIComponentBase.h"

namespace Engine {
	namespace Scripting {

		template <class T>
		class GlobalAPIComponent : public UniqueComponent<T, GlobalAPIComponentBase>, public GlobalAPI<T>, public Hierarchy::HierarchyObject<T> {
		public:
			using UniqueComponent::UniqueComponent;

			virtual const char *getName() override {
				return typeid(T).name();
			}
		};


	}
}