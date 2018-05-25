#include <gtest/gtest.h>

#include "Madgine/interfaceslib.h"

#include "Madgine/serialize/streams/debugging/streamdebugging.h"

TEST(StreamDebugging, Logging)
{
	Engine::Serialize::Debugging::StreamDebugging::setLoggingEnabled(true);
}
