#include "madginelib.h"

#include "OgreStreamReader.h"

namespace Engine {
		namespace Serialize {

			OgreStreamReader::OgreStreamReader(Ogre::DataStreamPtr stream) :
				mPtr(dynamic_cast<Ogre::MemoryDataStream*>(stream.get()))
			{
				setg((char*)mPtr->getPtr(), (char*)mPtr->getPtr(), (char*)mPtr->getPtr() + mPtr->size());
			}

			std::streampos OgreStreamReader::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)
			{
				char *base;
				switch (way) {
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
				char *newPos = base + off;
				int dist = newPos - eback();
				setg((char*)mPtr->getPtr(), (char*)mPtr->getPtr() + dist, (char*)mPtr->getPtr() + mPtr->size());
				return dist;
			}

			std::streampos OgreStreamReader::seekpos(std::streampos pos, std::ios_base::openmode which)
			{
				setg((char*)mPtr->getPtr(), (char*)mPtr->getPtr() + pos, (char*)mPtr->getPtr() + mPtr->size());
				return pos;
			}

		}
}
