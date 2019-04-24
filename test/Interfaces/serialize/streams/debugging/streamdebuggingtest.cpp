#include <gtest/gtest.h>

#include "Interfaces/interfaceslib.h"

#include "Interfaces/serialize/streams/debugging/streamdebugging.h"

TEST(StreamDebugging, Logging)
{
	Engine::Serialize::Debugging::setLoggingEnabled(true);
}
