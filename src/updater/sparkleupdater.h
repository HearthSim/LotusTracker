#pragma once

// https://el-tramo.be/blog/mixing-cocoa-and-qt/
// All credits to Track o'bot - https://github.com/stevschmid/track-o-bot

class SparkleUpdater {
public:
  virtual ~SparkleUpdater() {}

  virtual void CheckForUpdatesNow() = 0;

  virtual void SetAutomaticallyChecksForUpdates( bool automaticallyChecks ) = 0;
  virtual bool AutomaticallyChecksForUpdates() = 0;
};
