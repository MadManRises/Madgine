//
//  osx_main.m
//  Madgine
//
//  Created by Martin Schütz  on 7/23/20.
//

#include <UIKit/UIKit.h>

#include "Madgine/rootlib.h"

#include "Modules/threading/workgroup.h"

#include "Madgine/root/root.h"


#if __has_feature(objc_arc)
#error "ARC is on!"
#endif

#include "../launcher.h"



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
    Engine::Root::Root root { argc, argv };
    return launch();
}
