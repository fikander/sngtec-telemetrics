#include <QtCore/QCoreApplication>
#include "Sensors/devproxy.h"
#include "Cloud/cloproxy.h"
#include "Configurator/configurator.h"

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    // TODO wczytywanie konfiga i podawanie argow do konstruktorow
    Configurator config;
    DevProxy device(&config);
    CloProxy cloud(&config);
    device.connectCloud(&cloud);
    cloud.connectDev(&device);
    qDebug() << "running..";
    return a.exec();
}
