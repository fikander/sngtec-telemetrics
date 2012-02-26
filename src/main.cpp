#include <QtCore/QCoreApplication>
#include "Sensors/devproxy.h"
#include "Cloud/cloproxy.h"
#include "Configurator/configurator.h"
#include "Logger/logger.h"


int main(int argc, char *argv[]) {
    // Since we don't have a debug build configuration,
    // this is commented for more convenient development
    // qInstallMsgHandler(Logger::getInstance()->loggingHandler);
    QCoreApplication a(argc, argv);

    Configurator config;
    CloProxy cloud(&config);
    QVector<DevProxy*> devices(config.devicesAmount);

    for (int i = 0; i < config.devicesAmount; i++) {
        DevProxy device(&config);
        device.connectCloud(&cloud);
        cloud.connectDev(&device);
        devices[i] = &device;
    }

    qDebug() << "running..";
    return a.exec();
}
