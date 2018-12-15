#include "Interfaces/commonlib.h"

#include "Interfaces/plugins/binaryinfo.h"

extern "C" DLL_EXPORT Engine::Plugins::BinaryInfo binaryInfo {

	BINARY_MAJOR_VERSION,
	BINARY_MINOR_VERSION,
	BINARY_PATCH_NUMBER,

	BINARY_SOURCE_ROOT,
	BINARY_BINARY_DIR,
	BINARY_FILENAME

};