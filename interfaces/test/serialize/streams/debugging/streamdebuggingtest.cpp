#include <gtest/gtest.h>

#include "interfaceslib.h"

#include "serialize/serializemanager.h"
#include "serialize/streams/debugging/streamdebugging.h"
#include "serialize/streams/bufferedstream.h"

#include <filesystem>
#include <fstream>

TEST(StreamDebugging, Logging) {
	Engine::Serialize::Debugging::StreamDebugging::setLoggingEnabled(true);

}
