#include "macosxnative.h"

#include <AppKit/NSWindow.h>
#include <AppKit/Appkit.h>

namespace MacOSXNative
{
    void removeUnwantedMenuItems()
    {
        // Remove "Show Tab Bar" option from the "View" menu if possible

        if ([NSWindow respondsToSelector:@selector(allowsAutomaticWindowTabbing)])
        {
            NSWindow.allowsAutomaticWindowTabbing = NO;
        }
    }

    bool isMouseCoalescingEnabled()
    {
        return NSEvent.isMouseCoalescingEnabled;
    }

    void setMouseCoalescingEnabled(bool enabled)
    {
        NSEvent.mouseCoalescingEnabled = enabled;
    }

    bool isDarkMode()
    {
        NSAppearance* apperance = NSAppearance.currentAppearance;
        if (@available(macOS 10.14, *)) {
            return apperance.name == NSAppearanceNameDarkAqua;
        }
    }
}
