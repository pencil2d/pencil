#ifndef MACOSXNATIVE_H
#define MACOSXNATIVE_H

namespace MacOSXNative
{
    void removeUnwantedMenuItems();

    bool isMouseCoalescingEnabled();
    void setMouseCoalescingEnabled(bool enabled);
}

#endif // MACOSXNATIVE_H
