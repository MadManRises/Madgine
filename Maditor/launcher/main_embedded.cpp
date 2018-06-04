#include "maditorlauncherlib.h"

#include "embeddedwrapper.h"

#include "shared/errorcodes.h"

#include "Madgine/serialize/container/noparent.h"

int main(int argc, char *argv[]) {
	if (argc < 2) return Maditor::Shared::ILLEGAL_ARGUMENTS;
	size_t id;
	std::stringstream ss(argv[1]);
	ss >> id;
	if (id == 0)
		return Maditor::Shared::ILLEGAL_ARGUMENTS;
	Engine::Serialize::NoParentUnit<Maditor::Launcher::EmbeddedWrapper> app(id);
	return app.run();
}
