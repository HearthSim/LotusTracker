#include "macsparkleupdater.h"

#include <Cocoa/Cocoa.h>
#include <Sparkle/Sparkle.h>

// All credits to Track o'bot - https://github.com/stevschmid/track-o-bot

class MacSparkleUpdater::Private
{
public:
  SUUpdater* updater;
};

MacSparkleUpdater::MacSparkleUpdater( const QString& url )
{
  d = new Private;

  d->updater = [SUUpdater sharedUpdater];
  [d->updater retain];

  NSURL* nsUrl = [NSURL URLWithString:
      [NSString stringWithUTF8String: url.toUtf8().data()]];
  [d->updater setFeedURL: nsUrl];
}

MacSparkleUpdater::~MacSparkleUpdater()
{
  [d->updater release];
  delete d;
}

void MacSparkleUpdater::CheckForUpdatesNow() 
{
  return [d->updater checkForUpdates:nil];
}

void MacSparkleUpdater::SetAutomaticallyChecksForUpdates( bool automaticallyChecks )
{
  if( automaticallyChecks ) {
    [d->updater setAutomaticallyChecksForUpdates:YES];
  } else {
    [d->updater setAutomaticallyChecksForUpdates:NO];
  }
}

bool MacSparkleUpdater::AutomaticallyChecksForUpdates()
{
  return [d->updater automaticallyChecksForUpdates] == YES;
}
