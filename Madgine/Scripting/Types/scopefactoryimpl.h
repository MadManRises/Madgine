#pragma once

#include "scope.h"
#include "scopefactory.h"

namespace Engine {
	namespace Scripting {

		template <class T, class Base = Scope>
		class ScopeFactoryImpl : public Base {
		public:
			using Base::Base;

			virtual ScopeClass getClassType() override {
				return ScopeClassType<T>::type;
			};

		private:

			class Factory : public ScopeFactory {
			public:
				Factory() : ScopeFactory(ScopeClassType<T>::type) {}

				virtual Scope *create(Serialize::SerializeInStream &in);

			};

			static Factory sFactory;

		};

	}
}