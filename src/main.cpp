#include <QtCore/QCoreApplication>
#include "Sensors/devproxy.h"
#include "Cloud/cloproxy.h"
#include "Configurator/configurator.h"
#include "Logger/loggingHandler.h"

int main(int argc, char *argv[]) {
    qInstallMsgHandler(loggingHandler);
    QCoreApplication a(argc, argv);

    Configurator config;
    DevProxy device(&config);
    CloProxy cloud(&config);
    device.connectCloud(&cloud);
    cloud.connectDev(&device);
    qDebug() << "running..";
    return a.exec();
}
