#pragma once

#include "valuetype.h"

namespace Engine {
	namespace Scripting {

		class INTERFACES_EXPORT ScopeFieldAccessor {
		public:
			ScopeFieldAccessor(Scope &scope, const std::string &fieldName);

		protected:
			Scope &mScope;
			const std::string mFieldName;			
		};

		class INTERFACES_EXPORT GlobalFieldAccessor : public ScopeFieldAccessor
		{
		public:
			GlobalFieldAccessor(const std::string &fieldName);

		};

		class INTERFACES_EXPORT SceneFieldAccessor : public ScopeFieldAccessor
		{
		public:
			SceneFieldAccessor(const std::string &fieldName);

		};


		template <class T, class Accessor>
		class FieldAccessor : public Accessor {
		public:
			using Accessor::Accessor;

			void operator= (T v) {
				this->mScope.accessVar(this->mFieldName) = v;
			}

			operator T () {
				return value();
			}

			T value() {
				const ValueType &val = this->mScope.getVar(this->mFieldName);
				return val.as<T>();
			}

		};

		template <class T, class Accessor>
		class DefaultFieldAccessor : public FieldAccessor<T, Accessor> {
		public:
			template <class... Ty>
			DefaultFieldAccessor(const T &defaultValue, Ty&&... args) :
				FieldAccessor<T, Accessor>(std::forward<Ty>(args)...),
				mDefaultValue(defaultValue) {
			}

			void operator= (T v) {
				this->mScope.accessVar(this->mFieldName) = v;
			}

			operator T () {
				return value();
			}

			T value() {
				ValueType &val = this->mScope.accessVar(this->mFieldName);
				return val.asDefault<T>(mDefaultValue);
			}

		private:
			T mDefaultValue;
		};


		template <class T>
		using DefaultGlobalAccessor = DefaultFieldAccessor<T, GlobalFieldAccessor>;
		template <class T>
		using DefaultSceneAccessor = DefaultFieldAccessor<T, SceneFieldAccessor>;
		template <class T>
		using DefaultScopeAccessor = DefaultFieldAccessor<T, ScopeFieldAccessor>;

		template <class T>
		using GlobalAccessor = FieldAccessor<T, GlobalFieldAccessor>;
		template <class T>
		using SceneAccessor = FieldAccessor<T, SceneFieldAccessor>;
		template <class T>
		using ScopeAccessor = FieldAccessor<T, ScopeFieldAccessor>;

	}
}


