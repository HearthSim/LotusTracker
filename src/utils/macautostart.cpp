#include "macautostart.h"

#include <QApplication>
#include <QDir>
#include <QString>
#include <QSettings>
#include <QFileInfo>

// All credits to Track o'bot - https://github.com/stevschmid/track-o-bot

bool MacAutoStart::isEnabled()
{
    LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL, kLSSharedFileListSessionLoginItems, NULL);

    if(!loginItems) {
        return false;
    }

    UInt32 seed = 0U;
    CFArrayRef currentLoginItems = LSSharedFileListCopySnapshot(loginItems, &seed);
    LSSharedFileListItemRef existingItem = findLoginItemForCurrentBundle(currentLoginItems);

    bool isAutoRun = existingItem != NULL;
    CFRelease(currentLoginItems);
    CFRelease(loginItems);

    return isAutoRun;
}

void MacAutoStart::setEnabled(bool enabled)
{
    LSSharedFileListRef loginItems = LSSharedFileListCreate(NULL, kLSSharedFileListSessionLoginItems, NULL);

    if(!loginItems)
        return;

    UInt32 seed = 0U;
    CFArrayRef currentLoginItems = LSSharedFileListCopySnapshot(loginItems, &seed);
    LSSharedFileListItemRef existingItem = findLoginItemForCurrentBundle(currentLoginItems);

    if(enabled && (existingItem == NULL)) {
        CFURLRef mainBundleURL = CFBundleCopyBundleURL(CFBundleGetMainBundle());
        LSSharedFileListInsertItemURL(loginItems, kLSSharedFileListItemBeforeFirst, NULL, NULL, mainBundleURL, NULL, NULL);
        CFRelease(mainBundleURL);
    }
    else if(!enabled && (existingItem != NULL)) {
        LSSharedFileListItemRemove(loginItems, existingItem);
    }

    CFRelease(currentLoginItems);
    CFRelease(loginItems);
}

LSSharedFileListItemRef MacAutoStart::findLoginItemForCurrentBundle(CFArrayRef currentLoginItems)
{
    CFURLRef mainBundleURL = CFBundleCopyBundleURL(CFBundleGetMainBundle());

    for(int i = 0, end = CFArrayGetCount(currentLoginItems); i < end; ++i) {
        LSSharedFileListItemRef item = (LSSharedFileListItemRef)CFArrayGetValueAtIndex(currentLoginItems, i);

        UInt32 resolutionFlags = kLSSharedFileListNoUserInteraction | kLSSharedFileListDoNotMountVolumes;
        CFURLRef url = LSSharedFileListItemCopyResolvedURL(item, resolutionFlags, NULL);

        bool foundIt = CFEqual(url, mainBundleURL);
        CFRelease(url);

        if(foundIt) {
            CFRelease(mainBundleURL);
            return item;
        }
    }

    CFRelease(mainBundleURL);
    return NULL;
}
