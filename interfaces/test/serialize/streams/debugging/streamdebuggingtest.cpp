#include <gtest/gtest.h>

#include "interfaceslib.h"

#include "serialize/streams/debugging/streamdebugging.h"

TEST(StreamDebugging, Logging)
{
	Engine::Serialize::Debugging::StreamDebugging::setLoggingEnabled(true);
}
