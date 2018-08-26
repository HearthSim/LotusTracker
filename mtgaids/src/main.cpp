#include <QCoreApplication>
#include <QDebug>

#include "mtgaids.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    qDebug() << "Starting";
    MtgaIDs* mtgaIDs = new MtgaIDs();
    mtgaIDs->process();

    return app.exec();
}