#include "ogrelib.h"

#include "ogrestreamreader.h"

namespace Engine
{
	namespace Serialize
	{
		OgreStreamReader::OgreStreamReader(Ogre::DataStreamPtr stream) :
			mPtr(dynamic_cast<Ogre::MemoryDataStream*>(stream.get()))
		{
			setg(reinterpret_cast<char*>(mPtr->getPtr()), reinterpret_cast<char*>(mPtr->getPtr()),
			     reinterpret_cast<char*>(mPtr->getPtr()) + mPtr->size());
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
			setg(reinterpret_cast<char*>(mPtr->getPtr()), reinterpret_cast<char*>(mPtr->getPtr()) + dist,
			     reinterpret_cast<char*>(mPtr->getPtr()) + mPtr->size());
			return dist;
		}

		std::streampos OgreStreamReader::seekpos(std::streampos pos, std::ios_base::openmode which)
		{
			setg(reinterpret_cast<char*>(mPtr->getPtr()), reinterpret_cast<char*>(mPtr->getPtr()) + pos,
			     reinterpret_cast<char*>(mPtr->getPtr()) + mPtr->size());
			return pos;
		}
	}
}
