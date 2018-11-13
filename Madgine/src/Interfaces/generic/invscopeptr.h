#pragma once

namespace Engine
{
	
		class InvScopePtr
	{
	public:
		InvScopePtr() = default;

		InvScopePtr(Scripting::ScopeBase* ptr) : mPtr(ptr)
		{
		}

		Scripting::ScopeBase* validate() const { return mPtr; }
		operator bool() const { return mPtr != nullptr; }
		bool operator<(const InvScopePtr& other) const { return mPtr < other.mPtr; }
		bool operator==(const InvScopePtr& other) const { return mPtr == other.mPtr; }
	private:
		Scripting::ScopeBase* mPtr;
	};

}