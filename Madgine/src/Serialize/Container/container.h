#pragma once

#include "templates.h"
#include "valuetype.h"
#include "../serializemanager.h"

namespace Engine {
	namespace Serialize {
		enum Operations {
			ADD_ITEM,
			CLEAR
		};

		template <class T, bool b = std::is_base_of<SerializableUnit, T>::value>
		struct UnitHelper {

			static void read(SerializeInStream &in, T &item) {
				item.readState(in);
			}

			static void write(SerializeOutStream &out, const T &item) {
				item.writeCreationData(out);
				item.writeState(out);
			}

			static bool filter(SerializeOutStream &out, const T &item) {
				return true;
			}
		};

		template <class T, bool b>
		struct UnitHelper<Ogre::unique_ptr<T>, b> {

			static void read(SerializeInStream &in, Ogre::unique_ptr<T> &item) {
				UnitHelper<T>::read(in, *item);
			}

			static void write(SerializeOutStream &out, const Ogre::unique_ptr<T> &item) {
				UnitHelper<T>::write(out, *item);
			}

			static bool filter(SerializeOutStream &out, const Ogre::unique_ptr<T> &item) {
				return UnitHelper<T>::filter(out, *item);
			}
		};

		template <class T>
		struct UnitHelper<T, true> {

			static void read(SerializeInStream &in, T &item) {
				item.readState(in);
				InvPtr id;
				in >> id;
				in.manager().addMapping(id, &item);
				item.setMasterId(id);				
			}

			static void write(SerializeOutStream &out, const T &item) {
				item.writeCreationData(out);
				item.writeState(out);
				out << static_cast<InvPtr>(reinterpret_cast<uintptr_t>(static_cast<const SerializableUnit*>(&item)));
			}

			static bool filter(SerializeOutStream &out, const T &item) {
				return out.manager().filter(&item, out.id());
			}
		};


		template <bool areValueTypes, class... Args>
		class CreationHelper;

		template <class... Args>
		class CreationHelper<true, Args...> {
		protected:
			typedef std::tuple<Args...> ArgsTuple;

			ArgsTuple defaultCreationData(SerializeInStream &in) {
				ArgsTuple tuple;
				TupleLoader::loadTuple(tuple, in);
				return tuple;
			}
		};

		template <class... Args>
		class CreationHelper<false, Args...> {
		protected:
			std::tuple<Args...> defaultCreationData(SerializeInStream &in) {
				throw 0;
			}
		};

		template <class T, class... Args>
		class Container : protected CreationHelper<all_of<ValueType::isValueType<Args>::value...>::value, Args...>, protected UnitHelper<T> {
		protected:
			typedef std::tuple<Args...> ArgsTuple;
			typedef std::function<ArgsTuple(SerializeInStream&)> Factory;

			Container() {

			}

			template <class P, class... _Ty>
			Container(P *parent, ArgsTuple(P::*factory)(_Ty...))
			{
				mCreationDataFactory = [=](SerializeInStream &in) {
					std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> args;
					TupleLoader::loadTuple(args, in);
					return TupleUnpacker<>::call(parent, factory, args); };
			}

			ArgsTuple readCreationData(SerializeInStream &in) {
				if (mCreationDataFactory) {
					return mCreationDataFactory(in);
				}
				else {
					return defaultCreationData(in);
				}
			}

		private:
			Factory mCreationDataFactory;

		};

		
	}
}