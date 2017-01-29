#pragma once

#include "serializableunit.h"

namespace Engine {
namespace Serialize {

class MADGINE_EXPORT TopLevelSerializableUnit : public SerializableUnit
{
public:
	TopLevelSerializableUnit(Serialize::TopLevelMadgineObject type);
	virtual ~TopLevelSerializableUnit();	

	std::list<BufferedOutStream *> getMasterMessageTargets(SerializableUnit *unit);
	BufferedOutStream *getSlaveMessageTarget();

	SerializeManager *getSlaveManager();

	bool addManager(SerializeManager *mgr);
	void removeManager(SerializeManager *mgr);

	bool updateManagerType(SerializeManager *mgr, bool isMaster);

	bool isMaster();

	virtual TopLevelMadgineObject topLevelType() override;

private:
	std::list<SerializeManager*> mMasterManagers;
	SerializeManager *mSlaveManager;
	TopLevelMadgineObject mType;

};

} // namespace Serialize
} // namespace Core

