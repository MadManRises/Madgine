#pragma once

#include "serializable.h"
#include "serializablebase.h"

namespace Engine {
namespace Serialize {

class MADGINE_EXPORT SerializableUnit
{
public:
	SerializableUnit();
	SerializableUnit(const SerializableUnit &) = delete;
	virtual ~SerializableUnit();	

	std::list<BufferedOutStream *> getMasterMessageTargets(bool isAction);
	void writeMasterMessageHeader(BufferedOutStream &out, bool isAction);
	BufferedOutStream *getSlaveMessageTarget();

	virtual void writeCreationData(SerializeOutStream &out) const;
	virtual void writeState(SerializeOutStream &out) const;
	void writeId(SerializeOutStream &out) const;

	void readId(SerializeInStream &in);
	virtual void readState(SerializeInStream &in);
	void readAction(BufferedInOutStream &in);
	void readRequest(BufferedInOutStream &in);

	int addObservable(Observable* val);
	void addSerializableValue(Serializable *val);

	void setTopLevel(TopLevelSerializableUnit *topLevel);
	TopLevelSerializableUnit *topLevel();

	void clearMasterId();
	InvPtr masterId();
	/*void setMasterId(InvPtr id);*/

	void applySerializableMap(const std::map <InvPtr, SerializableUnit*> &map);

	//void writeHeader(SerializeOutStream &out, bool isAction);	

	virtual TopLevelMadgineObject topLevelType();
	

private:
	std::vector<Observable*> mObservedValues;
	std::vector<Serializable*> mStateValues;

	TopLevelSerializableUnit *mTopLevel;

	InvPtr mMasterId;
};

} // namespace Serialize
} // namespace Core

