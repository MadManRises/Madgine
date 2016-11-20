
#include "libinclude.h"

#include "Base/ApplicationLauncher.h"

#include "Shared.h"

SharedMemory *SharedMemory::msSingleton = 0;

int main() {

	SharedMemory mem(open);

	ApplicationLauncher app;
	return app.exec();

}