#pragma once


namespace Engine {
namespace Render {

    class MADGINE_CLIENT_EXPORT RenderWindow {
    public:
        RenderWindow(Window::Window *w);
        virtual ~RenderWindow();

        virtual void render() = 0;

        virtual std::unique_ptr<RenderTarget> createRenderTarget(Scene::Camera *camera, const Vector2 &size) = 0;

        void addRenderTarget(RenderTarget *target);
        void removeRenderTarget(RenderTarget *target);

        void beginFrame();
        void endFrame();

		virtual void makeCurrent() = 0;

    protected:

        void updateRenderTargets();

        Window::Window *window();

    private:
        Window::Window *mWindow;
        std::vector<RenderTarget *> mRenderTargets;
    };

}
}