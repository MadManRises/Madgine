#pragma once

#include "../renderpass.h"

#include "Madgine/pipelineloader/pipelineloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT BlurPass : RenderPass {

        BlurPass(int priority);
        
        virtual void setup(RenderTarget *target) override;
        virtual void shutdown() override;
        virtual void render(RenderTarget *target, size_t iteration) override;
        virtual void onTargetResize(const Vector2i &size) override;

        virtual int priority() const override;

        void setInput(RenderTarget *input, size_t inputIndex = 0);

    private:
        int mPriority; 

        size_t mInputIndex;
        RenderTarget *mInput;

        PipelineLoader::Instance mPipeline;
    };

}
}