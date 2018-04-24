#include "MacOSWindowFinder.h"

#include <ApplicationServices/ApplicationServices.h>
#include <QtMac>

MacOSWindowFinder::MacOSWindowFinder()
{
}

int MacOSWindowFinder::FindWindowId(const QString& name, const QString& title)
{
  int wId = 0;

  CFArrayRef windowList = CGWindowListCopyWindowInfo(kCGWindowListExcludeDesktopElements, kCGNullWindowID);
  CFIndex numWindows = CFArrayGetCount(windowList);
  CFStringRef nameRef = name.toCFString();
  CFStringRef titleRef = title == NULL ? name.toCFString() : title.toCFString();

  for(int i = 0; i < (int)numWindows; i++) {
    CFDictionaryRef info = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);
    CFStringRef thisWindowName = (CFStringRef)CFDictionaryGetValue(info, kCGWindowName);
    CFStringRef thisWindowOwnerName = (CFStringRef)CFDictionaryGetValue(info, kCGWindowOwnerName);

    if(thisWindowOwnerName && CFStringCompare(thisWindowOwnerName, nameRef, 0) == kCFCompareEqualTo) {
      if(thisWindowName && CFStringCompare(thisWindowName, titleRef, 0) == kCFCompareEqualTo) {
        CFNumberRef thisWindowNumber = (CFNumberRef)CFDictionaryGetValue(info, kCGWindowNumber);
        CFNumberGetValue(thisWindowNumber, kCFNumberIntType, &wId);
        break;
      }
    }
  }

  CFRelease(nameRef);
  CFRelease(windowList);
  return wId;
}

bool MacOSWindowFinder::isWindowFocused(int wId)
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
