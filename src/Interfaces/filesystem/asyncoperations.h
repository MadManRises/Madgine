#pragma once

#include "Generic/bytebuffer.h"
#include "Generic/functor.h"
#include "Generic/genericresult.h"
#include "Generic/execution/virtualstate.h"
#include "Generic/execution/concepts.h"
#include "path.h"

namespace Engine {
namespace Filesystem {

    struct AsyncFileReadAuxiliaryData;

    struct INTERFACES_EXPORT AsyncFileReadState : Execution::VirtualReceiverBase<GenericResult, ByteBuffer> {
        
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

    struct AsyncFileRead {
        using is_sender = void;

        using result_type = GenericResult;
        template <template <typename...> typename Tuple>
        using value_types = Tuple<ByteBuffer>;

        template <typename Rec>
        friend auto tag_invoke(Execution::connect_t, AsyncFileRead &&read, Rec &&rec)
        {
            return Execution::VirtualStateEx<Rec, AsyncFileReadState, type_pack<GenericResult>, ByteBuffer> {
                std::forward<Rec>(rec),
                std::move(read.mPath)
            };
        }
        Path mPath;
    };

}
}