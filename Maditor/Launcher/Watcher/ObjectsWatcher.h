#pragma once

#include "ObjectsInfo.h"


class ObjectsWatcher {


public:
	ObjectsWatcher();

	void update();

private:
	ObjectsShared &mShared;
};

