#include "../interfaceslib.h"
#include "serializableunit.h"

#include "streams/bufferedstream.h"

#include "serializemanager.h"

#include "toplevelserializableunit.h"

#include "observable.h"

#include "serializeexception.h"

#include "serializable.h"

namespace Engine
{
	namespace Serialize
	{
		thread_local std::list<SerializableUnitBase*> sStack;

		SerializableUnitBase::SerializableUnitBase(size_t masterId)
		{
			if (masterId == 0)
			{
				mMasterId = SerializeManager::addMasterMapping(this);
			}
			else
			{
				mMasterId = SerializeManager::addStaticMasterMapping(this, masterId);
			}
			insertInstance();
		}

		SerializableUnitBase::SerializableUnitBase(const SerializableUnitBase& other)
		{
			mMasterId = SerializeManager::addMasterMapping(this);
			insertInstance();
		}

		SerializableUnitBase::SerializableUnitBase(SerializableUnitBase&& other) noexcept
		{
			mMasterId = SerializeManager::updateMasterMapping(other.mMasterId, this);
			insertInstance();
		}

		SerializableUnitBase::~SerializableUnitBase()
		{
			if (find(sStack.begin(), sStack.end(), this) != sStack.end())
			{
				removeInstance();
			}

			assert(!mSynced);

			clearSlaveId();
			SerializeManager::removeMasterMapping(mMasterId, this);
		}

		void SerializableUnitBase::writeState(SerializeOutStream& out) const
		{
			for (Serializable* val : mStateValues)
			{
				val->writeState(out);
			}
		}

		void SerializableUnitBase::writeId(SerializeOutStream& out) const
		{
			out << mMasterId.first;
		}

		size_t SerializableUnitBase::readId(SerializeInStream& in)
		{
			size_t id;
			in >> id;
			if (&in.manager() == topLevel()->getSlaveManager())
			{
				setSlaveId(id);
			}
			return id;
		}

		void SerializableUnitBase::readState(SerializeInStream& in)
		{
			for (Serializable* val : mStateValues)
			{
				val->readState(in);
			}
		}

		void SerializableUnitBase::readAction(BufferedInOutStream& in)
		{
			size_t index;
			in >> index;
			try
			{
				mObservedValues.at(index)->readAction(in);
			}
			catch (const std::out_of_range&)
			{
				throw SerializeException("Unknown Observed-Id used! Possible binary mismatch!");
			}
		}

		void SerializableUnitBase::readRequest(BufferedInOutStream& in)
		{
			size_t index;
			in >> index;
			try
			{
				mObservedValues.at(index)->readRequest(in);
			}
			catch (const std::out_of_range&)
			{
				throw SerializeException("Unknown Observed-Id used! Possible binary mismatch!");
			}
		}

		size_t SerializableUnitBase::addObservable(Observable* val)
		{
			mObservedValues.push_back(val);
			return mObservedValues.size() - 1;
		}

		void SerializableUnitBase::addSerializable(Serializable* val)
		{
			mStateValues.push_back(val);
		}

		std::set<BufferedOutStream*, CompareStreamId> SerializableUnitBase::getMasterMessageTargets()
		{
			std::set<BufferedOutStream *, CompareStreamId> result;
			if (mSynced && mParent)
			{
				result = mParent->getMasterMessageTargets();
			}
			return result;
		}

		BufferedOutStream* SerializableUnitBase::getSlaveMessageTarget() const
		{
			BufferedOutStream* result = nullptr;
			if (mSynced)
			{
				result = topLevel()->getSlaveMessageTarget();
			}
			return result;
		}


		void SerializableUnitBase::writeCreationData(SerializeOutStream& out) const
		{
		}

		const TopLevelSerializableUnitBase* SerializableUnitBase::topLevel() const
		{
			return mParent ? mParent->topLevel() : nullptr;
		}

		void SerializableUnitBase::clearSlaveId()
		{
			if (mSlaveId != 0)
			{
				topLevel()->getSlaveManager()->removeSlaveMapping(this);
				mSlaveId = 0;
			}
		}

		void SerializableUnitBase::postConstruct()
		{
			removeInstance();
		}

		void SerializableUnitBase::setParent(SerializableUnitBase* parent)
		{
			clearSlaveId();
			mParent = parent;
		}

		void SerializableUnitBase::insertInstance()
		{
			findParentIt(this, this + 1);
			sStack.emplace_front(this);
			//std::cout << "Stack size: " << sStack.size() << std::endl;
		}

		void SerializableUnitBase::removeInstance()
		{
			assert(*sStack.begin() == this);
			//if (it != intern::stack.end()) {
			sStack.erase(sStack.begin());
			//std::cout << "Stack size: " << sStack.size() << std::endl;
			//}
		}

		SerializableUnitBase* SerializableUnitBase::findParent(void* from, void* to)
		{
			auto it = findParentIt(from, to);
			if (it != sStack.end())
			{
				return *it;
			}
			return nullptr;
		}

		std::list<SerializableUnitBase*>::iterator SerializableUnitBase::findParentIt(void* from, void* to)
		{
			auto it = std::find_if(sStack.begin(), sStack.end(), [&](SerializableUnitBase* const p)
			{
				return p <= from && to <= reinterpret_cast<char*>(p) + p->getSize();
			});
			if (it != sStack.end())
			{
				assert(it == sStack.begin());
				sStack.erase(sStack.begin(), it);
			}
			return it;
		}

		size_t SerializableUnitBase::getSize() const
		{
			return sizeof(SerializableUnitBase);
		}


		size_t SerializableUnitBase::slaveId() const
		{
			return mSlaveId;
		}

		size_t SerializableUnitBase::masterId() const
		{
			return mMasterId.first;
		}

		void SerializableUnitBase::setSlaveId(size_t id)
		{
			if (mSlaveId != id)
			{
				if (mSlaveId != 0)
				{
					clearSlaveId();
				}
				mSlaveId = id;
				topLevel()->getSlaveManager()->addSlaveMapping(this);
			}
		}

		void SerializableUnitBase::applySerializableMap(const std::map<size_t, SerializableUnitBase*>& map)
		{
			for (Serializable* ser : mStateValues)
			{
				ser->applySerializableMap(map);
			}
		}

		void SerializableUnitBase::setSynced(bool b)
		{
			setDataSynced(b);
			setActive(b);
		}

		void SerializableUnitBase::setDataSynced(bool b)
		{
			assert(mSynced != b);
			mSynced = b;
			for (Serializable* ser : mStateValues)
			{
				ser->setDataSynced(b);
			}
		}

		void SerializableUnitBase::setActive(bool active)
		{
			assert(mSynced == active);
			for (Serializable* ser : mStateValues)
			{
				ser->setActive(active);
			}
		}

		void SerializableUnitBase::sync()
		{
			setSynced(true);
		}

		void SerializableUnitBase::unsync()
		{
			setSynced(false);
		}

		bool SerializableUnitBase::isSynced() const
		{
			return mSynced;
		}

		bool SerializableUnitBase::isMaster() const
		{
			return mSlaveId == 0;
		}

		bool SerializableUnitBase::filter(SerializeOutStream* stream) const
		{
			return true;
		}
	} // namespace Serialize
} // namespace Core
