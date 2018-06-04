#include "../maditorlauncherlib.h"

#include "standalonewrapper.h"

#include "Madgine/serialize/container/noparent.h"

int main(int argc, char *argv[]) {
	Engine::Serialize::NoParentUnit<Maditor::Launcher::StandaloneWrapper> app;
	return app.listen(1234);
}
