#pragma once

namespace Engine {
		namespace Serialize {



			enum TopLevelMadgineObject {	
				NO_TOP_LEVEL,
				SERIALIZE_MANAGER,
				SCENE_MANAGER,
				SCRIPTING_MANAGER,
				MADITOR
			};

enum MessageType {
	STATE,
	ACTION,
	REQUEST
};

struct MessageHeader {
	MessageType mType;
	bool mIsMadgineComponent;
	union {
		InvPtr mObject;
		TopLevelMadgineObject mMadgineComponent;
	};
};

		}
}
