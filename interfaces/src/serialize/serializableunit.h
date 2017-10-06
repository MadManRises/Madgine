#pragma once

namespace Engine {
namespace Serialize {

class INTERFACES_EXPORT SerializableUnitBase
{
protected:

	SerializableUnitBase(TopLevelSerializableUnitBase *topLevel, size_t masterId = 0);
	SerializableUnitBase(TopLevelSerializableUnitBase *topLevel, const SerializableUnitBase &other);
	SerializableUnitBase(TopLevelSerializableUnitBase *topLevel, SerializableUnitBase &&other);
	virtual ~SerializableUnitBase();

public:
	TopLevelSerializableUnitBase *topLevel() const;

	virtual void writeState(SerializeOutStream &out) const;
	virtual void readState(SerializeInStream &in);

	void readAction(BufferedInOutStream &in);
	void readRequest(BufferedInOutStream &in);

	void writeId(SerializeOutStream &out) const;
	virtual size_t readId(SerializeInStream &in);

	void applySerializableMap(const std::map <size_t, SerializableUnitBase*> &map);

	virtual void writeCreationData(SerializeOutStream &out) const;

	size_t slaveId();
	size_t masterId();	


	virtual void onActivate();

	bool isActive() const;

protected:
	void activate();
	void deactivate();

	
private:

	std::list<BufferedOutStream *> getMasterMessageTargets(bool isActionconst, const std::list<ParticipantId> &targets = {});
	BufferedOutStream *getSlaveMessageTarget();	


	size_t addObservable(Observable* val);
	void addSerializable(Serializable *val);

	void setSlaveId(size_t id);
	void clearSlaveId();
	
	
	//void writeHeader(SerializeOutStream &out, bool isAction);		

	friend class SerializeManager;
	friend class Serializable;
	friend class Observable;
	friend class TopLevelSerializableUnitBase;
	template <class T, bool b>
	friend struct UnitHelper;
	template <class T>
	friend class SerializedUnit;
	/*template <template <class...> class C, class Creator, class T>
	friend class SerializableContainer;
	*/

private:
	std::vector<Observable*> mObservedValues;
	std::vector<Serializable*> mStateValues;

	TopLevelSerializableUnitBase *mTopLevel;

	size_t mSlaveId;
	std::pair<size_t, SerializableUnitMap*> mMasterId;

	bool mActive;

	////Stack

public:
	template <class T>
	static SerializableUnitBase *findParent(T *t) {
		return findParent(t, t + 1);
	}

protected:
	void postConstruct();

private:
	void insertInstance();
	void removeInstance();

	static SerializableUnitBase * findParent(void *from, void *to);
	static std::list<SerializableUnitBase*>::iterator findParentIt(void *from, void *to);

	virtual size_t getSize() const = 0;

	struct intern {
		static thread_local std::list<SerializableUnitBase*> stack;
	};
};

template <class T, class Base = SerializableUnitBase>
class SerializableUnit : public Base {
protected:
	using Base::Base;
	
	virtual size_t getSize() const override {
		return sizeof(T);
	}
};

} // namespace Serialize
} // namespace Core

