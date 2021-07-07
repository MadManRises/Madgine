#include "../moduleslib.h"
#include "connection.h"

namespace Engine {
namespace Threading {
    void ConnectionBase::disconnect()
    {
        mNode.unlink();
    }

}
}
