#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include "Modules/serialize/streams/debugging/streamdebugging.h"

TEST(StreamDebugging, Logging)
{
	Engine::Serialize::Debugging::setLoggingEnabled(true);
}
