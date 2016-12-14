#pragma once

#include "serializableunit.h"

namespace Engine {
namespace Serialize {

class MADGINE_EXPORT TopLevelSerializableUnit : public SerializableUnit
{
public:
	TopLevelSerializableUnit(Serialize::TopLevelMadgineObject type);
	virtual ~TopLevelSerializableUnit();	

	std::list<SerializeOutStream *> getMasterMessageTargets(SerializableUnit *unit);
	SerializeOutStream *getSlaveMessageTarget();

	void addManager(SerializeManager *mgr);
	void removeManager(SerializeManager *mgr);

	bool isMaster();

private:
	std::list<SerializeManager*> mMasterManagers;
	SerializeManager *mSlaveManager;

};

} // namespace Serialize
} // namespace Core

