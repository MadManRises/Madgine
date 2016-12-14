#pragma once

#include "SharedBase.h"


		enum ApplicationCmd {
			APP_CREATED,
			APP_INITIALIZED,
			APP_STARTED,
			APP_STOPPED,
			APP_SHUTDOWN,
			APP_AFTER_SHUTDOWN,

			START_APP,
			STOP_APP,
			SHUTDOWN,

			RESIZE_WINDOW
		};

		struct ApplicationInfo {
			ApplicationInfo(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
				mMediaDir(SharedCharAllocator(mgr)),
				mProjectDir(SharedCharAllocator(mgr)) {}

			int mWindowWidth;
			int mWindowHeight;
			size_t mWindowHandle;
			SharedString mMediaDir;
			SharedString mProjectDir;
		};

		struct ApplicationMsg {
			ApplicationCmd mCmd;
		};
