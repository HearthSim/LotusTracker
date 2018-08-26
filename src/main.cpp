#include "lotustracker.h"
#include <QMessageBox>

#ifdef ASM_CRASH_REPORT
#include "asmCrashReport.h"
#endif

int main(int argc, char *argv[])
{
    LotusTracker arenaTracker(argc, argv);
#ifdef ASM_CRASH_REPORT
    qDebug("--- asmCrashReport started ---");
    asmCrashReport::setSignalHandler(QString(), [] (const QString &inFileName, bool inSuccess) {
      QString  message;
      if (inSuccess) {
         message = QStringLiteral("Sorry, %1 has crashed. A log file was written to:\n\n%2\n\n"
                                  "Please email this to mtgalotus@gmail.com." )
                 .arg(QCoreApplication::applicationName(), inFileName);
      } else {
         message = QStringLiteral("Sorry, %1 has crashed and we could not write a log file to:\n\n%2\n\n"
                                  "Please contact mtgalotus@gmail.com." )
                 .arg(QCoreApplication::applicationName(), inFileName);
      }
      QMessageBox::critical(nullptr, QObject::tr("%1 Crashed").arg(QCoreApplication::applicationName()), message);
   });
#endif
    return arenaTracker.run();
}
