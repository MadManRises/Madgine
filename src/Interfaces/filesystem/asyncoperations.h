#pragma once

#include "Generic/bytebuffer.h"
#include "Generic/functor.h"
#include "Generic/genericresult.h"
#include "Generic/execution/sender.h"
#include "Generic/execution/virtualreceiver.h"
#include "path.h"

namespace Engine {
namespace Filesystem {

    struct AsyncFileReadAuxiliaryData;

    struct INTERFACES_EXPORT AsyncFileReadState : Execution::VirtualReceiverBase<ByteBuffer, GenericResult> {
        AsyncFileReadState(Path &&path);
        AsyncFileReadState(const AsyncFileReadState &) = delete;
        AsyncFileReadState(AsyncFileReadState &&) = delete;
        ~AsyncFileReadState();

        AsyncFileReadState &operator=(const AsyncFileReadState &) = delete;
        AsyncFileReadState &operator=(AsyncFileReadState &&) = delete;

        void start();

        Path mPath;

        std::unique_ptr<AsyncFileReadAuxiliaryData> mData;

        ByteBuffer mBuffer;
    };

    struct AsyncFileReadSender {
        template <typename Rec>
        auto operator()(Rec &&rec)
        {
            return Execution::VirtualReceiverEx<Rec, AsyncFileReadState, ByteBuffer, GenericResult> {
                std::forward<Rec>(rec),
                std::move(mPath)
            };
        }
        Path mPath;
    };

    using AsyncFileRead = Execution::Sender<AsyncFileReadSender, ByteBuffer, GenericResult>;

}
}