//
//  osx_main.m
//  Madgine
//
//  Created by Martin Schütz  on 7/23/20.
//

#include <UIKit/UIKit.h>

#include "Madgine/clientlib.h"
#include "Madgine/baselib.h"

#include "Modules/threading/workgroup.h"

#include "Madgine/core/root.h"

#include "cli/parameter.h"

#if __has_feature(objc_arc)
#error "ARC is on!"
#endif

extern Engine::CLI::Parameter<bool> toolMode;

extern int launch(Engine::Window::MainWindow **topLevelPointer = nullptr);



@interface MyUIApplication : UIApplication
@end

static bool running = false;

@implementation MyUIApplication

- (BOOL) isRunning
{
  return running;
}

@end

int main(int argc, char * argv[])  {
    [NSAutoreleasePool new];
    [MyUIApplication sharedApplication];
    /*[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
        [NSApp finishLaunching];
        running = true;
        [NSApp activateIgnoringOtherApps:YES];*/
    
    Engine::Threading::WorkGroup workGroup("Launcher");
    Engine::Core::Root root { argc, argv };
    if (!toolMode) {
        return launch();
    } else {
        return root.errorCode();
    }
}
