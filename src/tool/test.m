#import <AppKit/NSEvent.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSFont.h>

void disableCoalescing() {
        [NSEvent setMouseCoalescingEnabled:false];
}


void enableCoalescing() {
        [NSEvent setMouseCoalescingEnabled:true];
}
