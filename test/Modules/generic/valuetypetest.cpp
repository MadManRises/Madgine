#include <gtest/gtest.h>

#include "Modules/moduleslib.h"

#include <future>

#include "Modules/keyvalue/scopebase.h"

#include "Modules/keyvalue/valuetype.h"


TEST(ValueType, Construct)
{
	using namespace Engine;
	using namespace std::literals::string_view_literals;

	ValueType test;
	test = "abc"sv;
	test.clear(); //?
	test = true;
	test = 3;
	test = (size_t)4;
	test = 5.0f;
	test = Vector2(6.1f, 6.2f);
	test = Vector3(7.1f, 7.2f, 7.3f);
}
