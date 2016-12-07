#pragma once

namespace Engine {
		namespace Serialize {



			enum TopLevelMadgineObject {
				NO_TOPLEVEL_ITEM,
				SCENE_MANAGER
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
