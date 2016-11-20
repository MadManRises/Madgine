#pragma once

#define BOOST_DATE_TIME_NO_LIB
#include <boost\interprocess\sync\interprocess_mutex.hpp>
#include <boost\interprocess\containers\deque.hpp>
#include <boost\interprocess\managed_shared_memory.hpp>

#include "OgreVector2.h"
#include "GUI\GUIEvents.h"


enum InputEventType {
	PRESS,
	RELEASE
};


typedef boost::interprocess::allocator<std::pair<InputEventType, Engine::GUI::MouseEventArgs>,
	boost::interprocess::managed_shared_memory::segment_manager> SharedMouseEventAllocator;

typedef boost::interprocess::deque<std::pair<InputEventType, Engine::GUI::MouseEventArgs>, SharedMouseEventAllocator> SharedMouseDeque;

typedef std::queue<std::pair<InputEventType, Engine::GUI::MouseEventArgs>, SharedMouseDeque> SharedMouseQueue;

typedef boost::interprocess::allocator<std::pair<InputEventType, Engine::GUI::KeyEventArgs>,
	boost::interprocess::managed_shared_memory::segment_manager> SharedKeyEventAllocator;

typedef boost::interprocess::deque<std::pair<InputEventType, Engine::GUI::KeyEventArgs>, SharedKeyEventAllocator> SharedKeyDeque;

typedef std::queue<std::pair<InputEventType, Engine::GUI::KeyEventArgs>, SharedKeyDeque> SharedKeyQueue;


struct InputShared {
	InputShared(boost::interprocess::managed_shared_memory::segment_manager *mgr) :
		mMouseQueue(SharedMouseEventAllocator(mgr)),
		mKeyQueue(SharedKeyEventAllocator(mgr)){}

	boost::interprocess::interprocess_mutex mMutex;
	bool mMove;
	Engine::GUI::MouseEventArgs mAccumulativeMouseMove;

	SharedMouseQueue mMouseQueue;
	SharedKeyQueue mKeyQueue;
};



