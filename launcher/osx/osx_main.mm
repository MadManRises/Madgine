//
//  osx_main.m
//  Madgine
//
//  Created by Martin Schütz  on 7/23/20.
//

#include <Cocoa/Cocoa.h>

#include "Madgine/clientlib.h"
#include "Madgine/baselib.h"

#include "Modules/threading/workgroup.h"

#include "Madgine/base/root.h"

#include "cli/parameter.h"

#include "Interfaces/util/standardlog.h"

#include "../main.h"

#if __has_feature(objc_arc)
#error "ARC is on!"
#endif


@interface MyNSApplication : NSApplication
@end

static bool running = false;

@implementation MyNSApplication

- (BOOL) isRunning
{
  return running;
}

@end

int main(int argc, char * argv[])  {
    [NSAutoreleasePool new];
    [MyNSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    
        [NSApp finishLaunching];
        running = true;
        [NSApp activateIgnoringOtherApps:YES];
    
    return desktopMain(argc, argv);
}
