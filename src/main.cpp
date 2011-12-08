#include <QtCore/QCoreApplication>
#include "devproxy.h"
#include "cloproxy.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    // TODO wczytywanie konfiga i podawanie argow do konstruktorow

    DevProxy device;
    CloProxy cloud;
    device.connectCloud(&cloud);
    cloud.connectDev(&device);
    return a.exec();
}
