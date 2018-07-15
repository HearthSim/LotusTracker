#pragma once

#include "sparkleupdater.h"

#include <QString>

// All credits to Track o'bot - https://github.com/stevschmid/track-o-bot

class WinSparkleUpdater : public SparkleUpdater {
public:
  WinSparkleUpdater( const QString& url );
  ~WinSparkleUpdater();

  virtual void CheckForUpdatesNow();

  virtual void SetAutomaticallyChecksForUpdates( bool automaticallyChecks );
  virtual bool AutomaticallyChecksForUpdates();
};
