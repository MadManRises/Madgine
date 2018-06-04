#include <serverlib.h>
#include "%header"

extern "C" %guard Engine::Server::BaseServer *create%name() {
	return new %game::%module::%name();
}

namespace %game {
	namespace %module {

		%name::%name()
		{

		}

	}
}