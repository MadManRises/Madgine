#include "../clientlib.h"

#include "ogrestreamreader.h"

namespace Engine
{
	namespace Serialize
	{
		OgreStreamReader::OgreStreamReader(Ogre::MemoryDataStreamPtr stream) :
			mStream(stream)
		{
			setg(reinterpret_cast<char*>(mStream->getPtr()), reinterpret_cast<char*>(mStream->getPtr()),
			     reinterpret_cast<char*>(mStream->getPtr()) + mStream->size());
		}

		std::streampos OgreStreamReader::seekoff(std::streamoff off, std::ios_base::seekdir way,
		                                         std::ios_base::openmode which)
		{
			char* base;
			switch (way)
			{
			case std::ios_base::cur:
				base = gptr();
				break;
			case std::ios_base::beg:
				base = eback();
				break;
			case std::ios_base::end:
				base = egptr();
				break;
			}
			char* newPos = base + off;
			int dist = newPos - eback();
			setg(reinterpret_cast<char*>(mStream->getPtr()), reinterpret_cast<char*>(mStream->getPtr()) + dist,
			     reinterpret_cast<char*>(mStream->getPtr()) + mStream->size());
			return dist;
		}

		std::streampos OgreStreamReader::seekpos(std::streampos pos, std::ios_base::openmode which)
		{
			setg(reinterpret_cast<char*>(mStream->getPtr()), reinterpret_cast<char*>(mStream->getPtr()) + pos,
			     reinterpret_cast<char*>(mStream->getPtr()) + mStream->size());
			return pos;
		}
	}
}
