#pragma once

namespace Engine {

namespace Render {

    struct MADGINE_CLIENT_EXPORT RenderData {

        RenderData(RenderContext *context);

        void update();

        RenderContext *context() const;

    protected:
        virtual void render() = 0;

    private:
        RenderContext *mContext;

        size_t mFrame = 0;
    };

}

}