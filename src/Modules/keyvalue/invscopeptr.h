#pragma once

namespace Engine
{
	
		struct InvScopePtr
	{	
		InvScopePtr() = default;

		InvScopePtr(ScopeBase* ptr) : mPtr(ptr)
		{
		}

		ScopeBase* validate() const { return mPtr; }
		operator bool() const { return mPtr != nullptr; }
		bool operator<(const InvScopePtr& other) const { return mPtr < other.mPtr; }
		bool operator==(const InvScopePtr& other) const { return mPtr == other.mPtr; }
	private:
		ScopeBase* mPtr;
	};

}