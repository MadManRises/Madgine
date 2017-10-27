#include <gtest/gtest.h>

#include "interfaceslib.h"

#include "network/networkmanager.h"

#include <future>

#include "scripting/types/scopebase.h"


TEST(ValueType, Construct) {
	using namespace Engine;

	ValueType test;
	test = "abc";
	test.clear(); //?
	test = true;
	test = 3;
	test = (size_t)4;
	test = 5.0f;
	test = Vector2(6.1f, 6.2f);
	test = Vector3(7.1f, 7.2f, 7.3f);
}
