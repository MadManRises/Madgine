#pragma once

#include "streams/comparestreamid.h"

namespace Engine
{
	namespace Serialize
	{
		namespace __serialized__impl__
		{
			template <class T>
			class SerializedUnit;
		}

		class INTERFACES_EXPORT SerializableUnitBase
		{
		protected:

			SerializableUnitBase(size_t masterId = 0);
			SerializableUnitBase(const SerializableUnitBase& other);
			SerializableUnitBase(SerializableUnitBase&& other) noexcept;
			virtual ~SerializableUnitBase();

		public:
			virtual const TopLevelSerializableUnitBase* topLevel() const;

			virtual void writeState(SerializeOutStream& out) const;
			virtual void readState(SerializeInStream& in);

			void readAction(BufferedInOutStream& in);
			void readRequest(BufferedInOutStream& in);

			void writeId(SerializeOutStream& out) const;
			virtual size_t readId(SerializeInStream& in);

			void applySerializableMap(const std::map<size_t, SerializableUnitBase*>& map);

			virtual void writeCreationData(SerializeOutStream& out) const;

			size_t slaveId() const;
			size_t masterId() const;

			bool isActive() const;
			bool isMaster() const;

		protected:
			void activate();
			void deactivate();

			void setActive(bool b);

			virtual bool filter(Stream* stream);

		private:

			virtual std::set<BufferedOutStream*, CompareStreamId> getMasterMessageTargets();
			BufferedOutStream* getSlaveMessageTarget() const;


			size_t addObservable(Observable* val);
			void addSerializable(Serializable* val);

			void setSlaveId(size_t id);
			void clearSlaveId();

			void setActiveFlag(bool b);
			void notifySetActive(bool active);


			//void writeHeader(SerializeOutStream &out, bool isAction);		

			friend class SerializeManager;
			friend class Serializable;
			friend class Observable;
			friend class TopLevelSerializableUnitBase;
			template <class T, bool b>
			friend struct UnitHelper;
			template <class T>
			friend class __serialized__impl__::SerializedUnit;
			/*template <template <class...> class C, class Creator, class T>
			friend class SerializableContainer;
			*/

		private:
			std::vector<Observable*> mObservedValues;
			std::vector<Serializable*> mStateValues;

			SerializableUnitBase* mParent;

			size_t mSlaveId;
			std::pair<size_t, SerializableUnitMap*> mMasterId;

			bool mActive;

			////Stack

		public:
			template <class T>
			static SerializableUnitBase* findParent(T* t)
			{
				return findParent(t, t + 1);
			}

		protected:
			void postConstruct();
			void setParent(SerializableUnitBase* parent);

		private:
			void insertInstance();
			void removeInstance();

			static SerializableUnitBase* findParent(void* from, void* to);
			static std::list<SerializableUnitBase*>::iterator findParentIt(void* from, void* to);

			virtual size_t getSize() const = 0;

			struct intern
			{
				static thread_local std::list<SerializableUnitBase*> stack;
			};
		};

		template <class T, class Base = SerializableUnitBase>
		class SerializableUnit : public Base
		{
		protected:
			using Base::Base;

			virtual size_t getSize() const override
			{
				return sizeof(T);
			}
		};
	} // namespace Serialize
} // namespace Core
