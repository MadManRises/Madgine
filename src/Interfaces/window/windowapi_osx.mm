#include "../interfaceslib.h"

#include "windowapi.h"
#include "windowsettings.h"

#include <Cocoa/Cocoa.h>

#if __has_feature(objc_arc)
#error "ARC is on!"
#endif

namespace Engine{
namespace Window{
struct OSXWindow;
}
}

@interface Cocoa_WindowListener : NSObject <NSWindowDelegate>{
    Engine::Window::OSXWindow *mWindow;
}

- (void)listen:(Engine::Window::OSXWindow *)window;
@end

namespace Engine {
namespace Window {

    DLL_EXPORT const PlatformCapabilities platformCapabilities {
        true,
        static_cast<float>([[NSScreen mainScreen] backingScaleFactor])
    };


    struct OSXWindow : OSWindow {
        OSXWindow(NSWindow *handle)
            : OSWindow((uintptr_t)handle)
            , mListener([[Cocoa_WindowListener alloc] init])
        {
            [mListener listen:this];
        }
        
        ~OSXWindow(){
            [mListener dealloc];
        }

        /*bool handle(const XEvent &e)
        {
            switch (e.type) {
            case ConfigureNotify: {
                const XConfigureEvent &xce = e.xconfigure;

                /* This event type is generated for a variety of
                       happenings, so check whether the window has been
                       resized. */
/*
                if (xce.width != mWidth || xce.height != mHeight) {
                    mWidth = xce.width;
                    mHeight = xce.height;
                    onResize(mWidth, mHeight);
                }
                break;
            }
            case ClientMessage: {
                const XClientMessageEvent &xcme = e.xclient;
                if (xcme.data.l[0] == WM_DELETE_WINDOW) {
                    onClose();
                }
                break;
            }
            case DestroyNotify:
                return false;
            }
            return true;
        }*/

        virtual InterfacesVector size() override
        {
            NSRect rect = [reinterpret_cast<NSWindow*>(mHandle) contentRectForFrameRect:[reinterpret_cast<NSWindow*>(mHandle) frame]];
            return {static_cast<int>(rect.size.width), static_cast<int>(rect.size.height)};
        }

        virtual InterfacesVector renderSize() override
        {
            //TODO
            return {static_cast<int>(size().x * platformCapabilities.mScalingFactor), static_cast<int>(size().y * platformCapabilities.mScalingFactor)};
        }

        virtual InterfacesVector pos() override
        {
            NSRect rect = [reinterpret_cast<NSWindow*>(mHandle) contentRectForFrameRect:[reinterpret_cast<NSWindow*>(mHandle) frame]];
            return {static_cast<int>(rect.origin.x), static_cast<int>([[reinterpret_cast<NSWindow*>(mHandle) screen] frame].size.height - rect.origin.y - rect.size.height)};
        }

        virtual InterfacesVector renderPos() override
        {
            return {static_cast<int>(pos().x * platformCapabilities.mScalingFactor), static_cast<int>(pos().y * platformCapabilities.mScalingFactor)};
        }

        virtual void setSize(const InterfacesVector &size) override
        {
            NSRect rect{static_cast<double>(pos().x), static_cast<double>(pos().y), static_cast<double>(size.x), static_cast<double>(size.y)};
            [reinterpret_cast<NSWindow*>(mHandle) setFrame: rect display: YES animate: NO];
        }

        virtual void setRenderSize(const InterfacesVector &size) override
        {
            setSize({static_cast<int>(size.x / platformCapabilities.mScalingFactor), static_cast<int>(size.y / platformCapabilities.mScalingFactor)});
        }

        virtual void setPos(const InterfacesVector &size) override
        {
        }

        virtual void setRenderPos(const InterfacesVector &size) override
        {
        }

        virtual void show() override
        {
            [reinterpret_cast<NSWindow*>(mHandle) makeKeyAndOrderFront:NSApp];
        }

        virtual bool isMinimized() override
        {
            return false;
        }

        virtual void focus() override
        {
        }

        virtual bool hasFocus() override
        {
            return true;
        }

        virtual void setTitle(const char *title) override
        {
        }

        virtual void destroy() override
        {
            [reinterpret_cast<NSWindow*>(mHandle) close];
        }
        
        virtual void captureInput() override
        {
            
        }
        
        virtual void releaseInput() override
        {
            
        }

        virtual bool isMaximized() override
        {
            return false;
        }
        
        virtual bool isFullscreen() override
        {
            return false;
        }
        
        virtual void update() override
        {
            NSEvent* ev;
            while (NULL != (ev = [NSApp nextEventMatchingMask: NSAnyEventMask untilDate: nil inMode: NSDefaultRunLoopMode dequeue: YES]))
            {
                switch([ev type]){
                    case NSEventTypeMouseMoved:
                    case NSEventTypeLeftMouseUp:
                    case NSEventTypeLeftMouseDown:
                    case NSEventTypeLeftMouseDragged:
                    {
                        const NSPoint screenLocation = [NSEvent mouseLocation];
                        InterfacesVector screenPos {static_cast<int>(platformCapabilities.mScalingFactor * screenLocation.x), static_cast<int>(platformCapabilities.mScalingFactor * ([[reinterpret_cast<NSWindow*>(mHandle) screen] frame].size.height - screenLocation.y))};
                        const NSPoint windowLocation = [reinterpret_cast<NSWindow*>(mHandle) convertPointFromScreen:screenLocation];
                        InterfacesVector windowPos {static_cast<int>(platformCapabilities.mScalingFactor * windowLocation.x), renderSize().y - static_cast<int>(platformCapabilities.mScalingFactor * windowLocation.y)};
                        
                        switch([ev type]){
                            case NSEventTypeMouseMoved:
                            case NSEventTypeLeftMouseDragged:
                                injectPointerMove(Input::PointerEventArgs{
                                    windowPos,
                                    screenPos,
                                    windowPos - mLastKnownMousePos });
                                mLastKnownMousePos = windowPos;
                                break;
                            case NSEventTypeLeftMouseDown:
                                injectPointerPress(Input::PointerEventArgs{
                                   windowPos, screenPos, Input::MouseButton::LEFT_BUTTON
                                });
                                break;
                            case NSEventTypeLeftMouseUp:
                                injectPointerRelease(Input::PointerEventArgs{
                                    windowPos, screenPos, Input::MouseButton::LEFT_BUTTON
                                });
                                break;
                            default:
                                std::terminate();
                        }
                    }
                        break;
                    default:
                        LOG_WARNING("Unhandled event: " << [ev type]);
                }
                
                [NSApp sendEvent: ev];
            }
        }
        
        virtual bool isKeyDown(Input::Key::Key key) override
        {
            return false;
        }
        
        virtual std::string title() const override
        {
            return "";
        }
        
        using OSWindow::onClose;
        using OSWindow::onRepaint;
        using OSWindow::onResize;

    private:
        InterfacesVector mLastKnownMousePos;
        Cocoa_WindowListener *mListener;
    };

}}


@implementation Cocoa_WindowListener
- (void)listen:(Engine::Window::OSXWindow *)window
{
    mWindow = window;
    [reinterpret_cast<NSWindow*>(window->mHandle) setDelegate:self];
}

- (void)windowDidResize:(NSNotification *)aNotification
{
    mWindow->onResize(mWindow->size());
}
@end

namespace Engine{
namespace Window{

    static std::unordered_map<NSWindow *, OSXWindow> sWindows;

    OSWindow *sCreateWindow(const WindowSettings &settings)
    {
        NSRect rect = NSMakeRect(0, 0, settings.mData.mSize.x, settings.mData.mSize.y);
        if (settings.mData.mPosition.x != -1 || settings.mData.mPosition.y != -1) {
            rect.origin.x = settings.mData.mPosition.x;
            rect.origin.y = settings.mData.mPosition.y;
        }
        
        id menubar = [NSMenu new];
            id appMenuItem = [NSMenuItem new];
            [menubar addItem:appMenuItem];
            [NSApp setMainMenu:menubar];
            id appMenu = [NSMenu new];
            id appName = [[NSProcessInfo processInfo] processName];
            id quitTitle = [@"Quit " stringByAppendingString:appName];
            id quitMenuItem = [[[NSMenuItem alloc] initWithTitle:quitTitle
            action:@selector(terminate:) keyEquivalent:@"q"] autorelease];
            [appMenu addItem:quitMenuItem];
            [appMenuItem setSubmenu:appMenu];
        
        NSWindowStyleMask style = 0;
        
        if (settings.mHeadless){
            style |= NSWindowStyleMaskBorderless;
        }else{
            style |= NSWindowStyleMaskTitled |
                     NSWindowStyleMaskClosable |
                     NSWindowStyleMaskResizable |
                     NSWindowStyleMaskMiniaturizable;
        }
        
        NSWindow *handle = [[NSWindow alloc] initWithContentRect:rect
                                             styleMask:style
                                             backing:NSBackingStoreBuffered
                                             defer:NO];


        [handle setTitle:[NSString stringWithUTF8String:settings.mTitle]];
        
        [handle cascadeTopLeftFromPoint:NSMakePoint(20,20)];
        [handle makeKeyAndOrderFront:nil];
        
        
        auto pib = sWindows.try_emplace(handle, handle);
        assert(pib.second);
        
        return &pib.first->second;
    }

    static std::vector<MonitorInfo> sBuffer;

    static void updateMonitors()
    {
        sBuffer.clear();
        for (NSScreen *screen in [NSScreen screens]){
            NSRect frame = [screen frame];
            sBuffer.push_back({static_cast<int>(frame.origin.x), static_cast<int>(frame.origin.y), static_cast<int>(frame.size.width), static_cast<int>(frame.size.height)});
        }
    }
    
    std::vector<MonitorInfo> listMonitors()
    {
        if (sBuffer.empty())
            updateMonitors();
        return sBuffer;
    }


}
}
