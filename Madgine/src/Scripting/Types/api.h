#pragma once

#include "Scripting\Datatypes\argumentlist.h"
#include "Scripting\Datatypes\valuetype.h"
#include "Database\exceptionmessages.h"
#include "Util\UtilMethods.h"
#include "Scripting\Types\scope.h"
#include "Scripting\scriptingexception.h"

namespace Engine {
	namespace Scripting {

		class BaseAPI {
		public:
			virtual ~BaseAPI() = default;

			virtual bool hasMethod(const std::string &name) const = 0;
			virtual ValueType execMethod(const std::string &name, const ArgumentList &list) = 0;
		};


		template <class _T>
		struct Caster {
			static auto cast(const ValueType &v) {
				return v.as<std::remove_const_t<std::remove_reference_t<_T>>>();
			}
		};

		template <>
		struct Caster<const ValueType &> {
			static auto cast(const ValueType &v) {
				return v;
			}
		};

		template <>
		struct Caster<List*> {
			static auto cast(const ValueType &v) {
				List *list = scope_cast<List>(v.asScope());
				if (!list)
					throw 0;
				return list;
			}
		};

		template <>
		struct Caster<Engine::Scene::Entity::Entity*> {
			static auto cast(const ValueType &v) {
				Engine::Scene::Entity::Entity *e = scope_cast<Engine::Scene::Entity::Entity>(v.asScope());
				if (!e)
					throw 0;
				return e;
			}
		};

		template<bool...> struct bool_pack;

		template<bool...values> struct all_of
			: std::is_same<bool_pack<values..., true>, bool_pack<true, values...>> {};

		template <class T>
		class API : public BaseAPI {
		public:
			virtual ~API() = default;

			virtual bool hasMethod(const std::string &name) const override {
				return sMethods.find(name) != sMethods.end();
			}

			virtual ValueType execMethod(const std::string &name, const ArgumentList &list) override {
				TRACE_S(sFile, -1, name);
				return (this->*sMethods.find(name)->second)(list);
			}

			typedef Engine::Scripting::ValueType(API::*Method)(const Engine::Scripting::ArgumentList&);

		private:			

			template <class R, class... _Ty>
			struct Impl {
				template <size_t... I>
				static ValueType call(R(T::*f)(_Ty...), T *my, const ArgumentList &list, const std::index_sequence<I...>) {
					return (my->*f)(Caster<_Ty>::cast(list.at(I))...);
				}

				template <size_t... I>
				static ValueType call(R(T::*f)(_Ty...) const, const T *my, const ArgumentList &list, const std::index_sequence<I...>) {
					return (my->*f)(Caster<_Ty>::cast(list.at(I))...);
				}

				template <size_t... I>
				static ValueType callVariadic(R(T::*f)(_Ty..., const ArgumentList &), T *my, const ArgumentList &list, const ArgumentList &variadic, const std::index_sequence<I...>) {
					return (my->*f)(Caster<_Ty>::cast(list.at(I))..., variadic);
				}

				template <size_t... I>
				static ValueType callVariadic(R(T::*f)(_Ty..., const ArgumentList &) const, const T *my, const ArgumentList &list, const ArgumentList &variadic, const std::index_sequence<I...>) {
					return (my->*f)(Caster<_Ty>::cast(list.at(I))..., variadic);
				}
			};

			template <class... _Ty>
			struct Impl<void, _Ty...> {
				template <size_t... I>
				static ValueType call(void(T::*f)(_Ty...), T *my, const ArgumentList &list, const std::index_sequence<I...>) {
					(my->*f)(Caster<_Ty>::cast(list.at(I))...);
					return ValueType();
				}

				template <size_t... I>
				static ValueType call(void(T::*f)(_Ty...) const, const T *my, const ArgumentList &list, const std::index_sequence<I...>) {
					(my->*f)(Caster<_Ty>::cast(list.at(I))...);
					return ValueType();
				}

				template <size_t... I>
				static ValueType callVariadic(void(T::*f)(const ArgumentList &, _Ty...), T *my, const ArgumentList &list, const ArgumentList &variadic, const std::index_sequence<I...>) {
					(my->*f)(variadic, Caster<_Ty>::cast(list.at(I))...);
					return ValueType();
				}

				template <size_t... I>
				static ValueType callVariadic(void(T::*f)(const ArgumentList &, _Ty...) const, const T *my, const ArgumentList &list, const ArgumentList &variadic, const std::index_sequence<I...>) {
					(my->*f)(variadic, Caster<_Ty>::cast(list.at(I))...);
					return ValueType();
				}
			};



			template <class R, class... _Ty>
			std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value...>::value, ValueType> wrapped(R(T::*f)(_Ty...), const ArgumentList &list) {
				if (list.size() != sizeof...(_Ty))
					MADGINE_THROW(ScriptingException(Database::Exceptions::argumentCountMismatch(sizeof...(_Ty), list.size())));
				return Impl<R, _Ty...>::call(f, static_cast<T*>(this), list, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class R, class... _Ty>
			std::enable_if_t<all_of<!std::is_same<std::remove_const_t<std::remove_reference_t<_Ty>>, ArgumentList>::value...>::value, ValueType> wrapped(R(T::*f)(_Ty...) const, const ArgumentList &list) {
				if (list.size() != sizeof...(_Ty))
					MADGINE_THROW(ScriptingException(Database::Exceptions::argumentCountMismatch(sizeof...(_Ty), list.size())));
				return Impl<R, _Ty...>::call(f, static_cast<T*>(this), list, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class R, class... _Ty>
			ValueType wrapped(R(T::*f)(const ArgumentList &list, _Ty...), const ArgumentList &list) {
				if (list.size() < sizeof...(_Ty))
					throw 0;
				auto it = list.begin() + sizeof...(_Ty);
				return Impl<R, _Ty...>::callVariadic(f, static_cast<T*>(this), { list.begin(), it }, { it, list.end() }, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class R, class... _Ty>
			ValueType wrapped(R(T::*f)(const ArgumentList &list, _Ty...) const, const ArgumentList &list) {
				if (list.size() < sizeof...(_Ty))
					throw 0;
				auto it = list.begin() + sizeof...(_Ty);
				return Impl<R, _Ty...>::callVariadic(f, static_cast<T*>(this), { list.begin(), it }, { it, list.end() }, std::make_index_sequence<sizeof...(_Ty)>());
			}

			template <class _F, _F _f>
			ValueType wrap(const ArgumentList &list) {
				return wrapped(_f, list);
			}




			template <class... _F>
			struct TypeCatcher {
				template <_F... _f>
				struct ValueCatcher {
					static const std::map<std::string, Method> value(const char *s = 0) {
						auto read = [&]() {
							s = strchr(s, '&') + 1;
							while (*s == ' ') ++s;
							const char *next = strchr(s, ',');
							if (next) {
								size_t len = next - s;
								while (s[len - 1] == ' ') {
									--len;
									assert(len > 0);
								}
								return std::string(s, len);
							}
							else {
								return std::string(s);

							}
						};
						return{ std::pair<const std::string, Method>(read(), &API::wrap<_F, _f>)... };
					}
				};
			};


			template <class... _F>
			static auto _type(_F... f) {
				using Type = TypeCatcher<_F...>;
				return Type();
			}

		private:
			static const std::map<std::string, Method> sMethods;
			static const char *sFile;

		};



#define API_IMPL(Class, ...) \
	template<> const char *Engine::Scripting::API<Class>::sFile = __FILE__; \
	const std::map<std::string, Class::Method> Class::sMethods = decltype(_type(__VA_ARGS__))::ValueCatcher<__VA_ARGS__>::value(#__VA_ARGS__)

	} // namespace Scripting
} // namespace Core