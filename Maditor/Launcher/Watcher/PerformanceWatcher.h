#pragma once



struct SharedStats;

class PerformanceWatcher {

public:
	PerformanceWatcher();

	void update();
	void clear();

private:
	SharedStats &mShared;

};

