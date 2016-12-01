
#include "madginelib.h"

#include "Base/ApplicationLauncher.h"

#include "Shared.h"


SharedMemory *SharedMemory::msSingleton = 0;

int main() {

	SharedMemory mem(SharedMemory::open);

	ApplicationLauncher app;
	return app.exec();

}