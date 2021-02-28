#include "networklib.h"

#include "networkbuffer.h"

#include "Meta/serialize/formatter.h"

namespace Engine {
namespace Network {
    NetworkBuffer::NetworkBuffer(SocketId socket)
        : mSocket(socket)
    {
    }

    /*NetworkBuffer::NetworkBuffer(NetworkBuffer &&other) noexcept
        : buffered_streambuf(std::move(other))
        , mSocket(std::exchange(other.mSocket, Invalid_Socket))
    {
    }*/

    NetworkBuffer::~NetworkBuffer()
    {
        if (mSocket != Invalid_Socket)
            SocketAPI::closeSocket(mSocket);
    }

    std::streamsize NetworkBuffer::xsgetn(char *buf, std::streamsize len)
    {
        int result = SocketAPI::recv(mSocket, buf, len);
        if (result == -1) {
            switch (SocketAPI::getError("recv")) {
            case SocketAPIResult::WOULD_BLOCK:
                return 0;
            default:
                return 0;
            }
        }
        return result;
    }

    std::streamsize NetworkBuffer::xsputn(const char *buf, std::streamsize len)
    {
        int result = SocketAPI::send(mSocket, buf, len);
        if (result == -1) {
            switch (SocketAPI::getError("recv")) {
            default:
                return 0;
            }
        }
        return result;
    }

    std::streamsize NetworkBuffer::showmanyc()
    {
        return SocketAPI::in_available(mSocket);
    }

    /*void NetworkBuffer::handleError()
    {
        switch (SocketAPI::getError("unknown")) {
        case SocketAPIResult::WOULD_BLOCK:
            break;
        default:
            close(Serialize::StreamState::UNKNOWN_ERROR);
        }
    }*/
}
}
