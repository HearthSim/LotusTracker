#include "macwindowfinder.h"

#include <ApplicationServices/ApplicationServices.h>
#include <QtMac>
#include "macros.h"

// All credits to Track o'bot - https://github.com/stevschmid/track-o-bot

MacWindowFinder::MacWindowFinder()
{
}

int MacWindowFinder::findWindowId(const QString& name)
{
    int wId = 0;

    CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListExcludeDesktopElements, kCGNullWindowID);
    CFIndex numWindows = CFArrayGetCount(windowList);
    CFStringRef nameRef = name.toCFString();

    for(int i = 0; i < (int)numWindows; i++) {
        CFDictionaryRef info = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);
        CFStringRef thisWindowName = (CFStringRef)CFDictionaryGetValue(info, kCGWindowName);
        CFStringRef thisWindowOwnerName = (CFStringRef)CFDictionaryGetValue(info, kCGWindowOwnerName);
//        LOGD(QString("%1 - %2")
//               .arg(QString::fromCFString(thisWindowName))
//               .arg(QString::fromCFString(thisWindowOwnerName)));
//        if(thisWindowOwnerName && CFStringCompare(thisWindowOwnerName, nameRef, 0) == kCFCompareEqualTo) {
            if(thisWindowName && CFStringCompare(thisWindowName, nameRef, 0) == kCFCompareEqualTo) {
                CFNumberRef thisWindowNumber = (CFNumberRef)CFDictionaryGetValue(info, kCGWindowNumber);
                CFNumberGetValue(thisWindowNumber, kCFNumberIntType, &wId);
                break;
            }
//        }
    }

    CFRelease(nameRef);
    CFRelease(windowList);
    return wId;
}

bool MacWindowFinder::isWindowFocused(int wId)
{
    CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionIncludingWindow, wId);
    CFIndex numWindows = CFArrayGetCount(windowList);

    bool focus = false;

    if(numWindows > 0) {
      CFDictionaryRef info = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, 0);
      CFBooleanRef thisWindowOnScreen = (CFBooleanRef)(CFDictionaryGetValue(info, kCGWindowIsOnscreen));
      if(thisWindowOnScreen != NULL) {
        focus = CFBooleanGetValue(thisWindowOnScreen);
      }
    }

    CFRelease(windowList);
    return focus;
}
