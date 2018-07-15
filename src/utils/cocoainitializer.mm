#include "cocoainitializer.h"

#include <AppKit/AppKit.h>
#include <Cocoa/Cocoa.h>

// All credits to Track o'bot - https://github.com/stevschmid/track-o-bot

class CocoaInitializer::Private 
{
public:
  NSAutoreleasePool* autoReleasePool_;
};

CocoaInitializer::CocoaInitializer()
{
  d = new CocoaInitializer::Private();
  NSApplicationLoad();
  d->autoReleasePool_ = [[NSAutoreleasePool alloc] init];
}

CocoaInitializer::~CocoaInitializer()
{
  [d->autoReleasePool_ release];
  delete d;
}
