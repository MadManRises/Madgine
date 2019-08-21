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

		class MODULES_EXPORT SerializableUnitBase
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

			bool isSynced() const;
			bool isMaster() const;

		protected:
			void sync();
			void unsync();

			void setSynced(bool b);

			virtual bool filter(SerializeOutStream* stream) const;

			void setSlaveId(size_t id);

		private:

			virtual std::set<BufferedOutStream*, CompareStreamId> getMasterMessageTargets() const;
			BufferedOutStream* getSlaveMessageTarget() const;

			void addSerializable(Serializable* val);

			void clearSlaveId();

			void setDataSynced(bool b);
			void setActive(bool active);


			//void writeHeader(SerializeOutStream &out, bool isAction);		

			friend struct SerializeManager;
			friend class Serializable;
			friend struct ObservableBase;
			//friend class TopLevelSerializableUnitBase;
			friend struct SerializeUnitHelper;
			template <class T>
			friend class __serialized__impl__::SerializedUnit;
			/*template <template <class...> class C, class Creator, class T>
			friend class SerializableContainer;
			*/

		private:
			std::vector<Serializable*> mStateValues;

			SerializableUnitBase* mParent = nullptr;

			size_t mSlaveId = 0;
			size_t mMasterId;

			bool mSynced = false;

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
		};

		template <class T, class Base = SerializableUnitBase>
		class SerializableUnit : public Base
		{
		protected:
			using Base::Base;

			using Self = T;

			virtual size_t getSize() const override
			{
				return sizeof(T);
			}
		};
	} // namespace Serialize
} // namespace Core
