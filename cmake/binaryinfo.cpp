

#include "Interfaces/plugins/binaryinfo.h"

extern "C" __declspec(dllexport) Engine::Plugins::BinaryInfo binaryInfo {

	BINARY_MAJOR_VERSION,
	BINARY_MINOR_VERSION,
	BINARY_PATCH_NUMBER,

	BINARY_SOURCE_ROOT,
	BINARY_BINARY_DIR,
	BINARY_FILENAME

};