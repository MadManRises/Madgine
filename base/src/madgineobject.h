#pragma once

#include "objectstate.h"

namespace Engine
{
	class MADGINE_BASE_EXPORT MadgineObject
	{
	protected:
		MadgineObject();
		virtual ~MadgineObject();

	public:

		virtual bool init();
		virtual void finalize();

		ObjectState getState() const;

	private:
		ObjectState mState;
		std::string mName;
	};
}
