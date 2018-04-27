#ifndef MACAUTOSTART_H
#define MACAUTOSTART_H

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

class MacAutoStart
{
private:
	static LSSharedFileListItemRef findLoginItemForCurrentBundle(CFArrayRef currentLoginItems);

public:
	static bool isEnabled();
    static void setEnabled(bool enabled);
};

#endif // MACAUTOSTART_H
