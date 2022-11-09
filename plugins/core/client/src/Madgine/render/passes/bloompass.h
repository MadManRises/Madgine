#pragma once

#include "../renderpass.h"
#include "blurpass.h"

namespace Engine {
namespace Render {

    struct MADGINE_CLIENT_EXPORT BloomPass : RenderPass {

        BloomPass(int priority);
        ~BloomPass();

        virtual void setup(RenderTarget *target) override;
        virtual void shutdown() override;
        virtual void render(RenderTarget *target, size_t iteration) override;        
        virtual void onTargetResize(const Vector2i &size) override;
        
        virtual int priority() const override;

        void setInput(RenderTarget *input, size_t inputIndex = 0, RenderTarget *blurInput = nullptr, size_t blurIndex = 1);

        float mExposure = 1.0f;

    private:
        int mPriority;

        std::unique_ptr<RenderTarget> mBlurTarget;

        BlurPass mBlur;

        size_t mInputIndex;
        RenderTarget *mInput;

        PipelineLoader::Instance mPipeline;
    };

}
}