#pragma once

namespace Engine {
	namespace Scripting {
		struct Mapper {
			Mapper() = delete;
			Mapper(ValueType(*toValueType)(ScopeBase *)) : mToValueType(toValueType) {}

			ValueType(*mToValueType)(ScopeBase *);


		private:
			template <class T, ValueType(*F)(T *, const ArgumentList&)>
			static ValueType map_f2(ScopeBase *s, const ArgumentList &args) {
				return F(dynamic_cast<T*>(s), args);
			}

		public:

			template <class T, ValueType(*F)(T *, const ArgumentList&)>
			static ValueType map_f(ScopeBase *) {
				return ValueType(&map_f2<T, F>);
			}
		};
	}
}