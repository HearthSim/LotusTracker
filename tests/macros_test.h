#ifndef MACROS_H

#include <QString>

#define READ_LOG(fileName, logRef) \
	QFile msgFile(QString(":/res/logs/%1").arg(fileName)); \
	if (!msgFile.open(QIODevice::ReadOnly | QFile::Text)) { \
		QFAIL("File not found."); \
	} \
	logRef = QTextStream(&msgFile).readAll(); \

#endif // MACROS_H