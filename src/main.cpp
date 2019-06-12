#include "lotustracker.h"
#include "utils/lotusexception.h"

#ifdef ASM_CRASH_REPORT
#include "asmCrashReport.h"
#endif

#include <crow/crow.hpp>
#include <QMessageBox>

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
      QFile logFile(inFileName);
      if (logFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
          logFile.seek(0);
          QByteArray logNewContent = logFile.readAll();
          QString content = QString::fromUtf8(logNewContent.trimmed());
          nlohmann::crow crow_client("https://a123:b456@sentry.io/c789");
          crow_client.capture_exception(LotusException(content));
      }
      QMessageBox::critical(nullptr, QObject::tr("%1 Crashed").arg(QCoreApplication::applicationName()), message);
   });
#endif
    return arenaTracker.run();
}
