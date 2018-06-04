#pragma once

#include "shared/moduleinstance.h"
#include "Madgine/serialize/container/action.h"
#include "Madgine/util/loglistener.h"
#include "Madgine/signalslot/slot.h"

namespace Maditor {
	namespace Launcher {

class LauncherLogListener : public Engine::Serialize::SerializableUnit<LauncherLogListener>
	, public Engine::Util::LogListener
#ifdef MADGINE_CLIENT_BUILD
	, public Ogre::LogListener 
#endif
{


public:
	LauncherLogListener();
	~LauncherLogListener();

	void init();

protected:
	// Inherited via LogListener
	virtual void messageLogged(const std::string & message, Engine::Util::MessageType lml, const std::list<Engine::Util::TraceBack> &traceback, const std::string & logName) override;

#ifdef MADGINE_CLIENT_BUILD
	virtual void messageLogged(const Ogre::String & message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String & logName, bool & skipThisMessage) override;
#endif
private:
	void receiveImpl(const std::string &msg, Engine::Util::MessageType level, const std::string &logName, const std::string &fullTraceback, const std::string &fileName, int lineNr);

private:
	Engine::Serialize::Action<decltype(&LauncherLogListener::receiveImpl), &LauncherLogListener::receiveImpl, Engine::Serialize::ActionPolicy::notification> receiveMessage;
	typedef Engine::SignalSlot::Slot<decltype(&decltype(LauncherLogListener::receiveMessage)::operator()), &decltype(LauncherLogListener::receiveMessage)::operator()> SlotType;
	std::unique_ptr<SlotType> mSlot;
	
};

	}
}
