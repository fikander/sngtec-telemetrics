#include <QtCore/QCoreApplication>
#include "Sensors/devproxy.h"
#include "Cloud/cloproxy.h"
#include "Configurator/configurator.h"
#include "Logger/logger.h"
#include "Sensors/Modbus/modbus.h"


int main(int argc, char *argv[]) {
    // Since we don't have a debug build configuration,
    // this is commented for more convenient development
    // qInstallMsgHandler(Logger::getInstance()->loggingHandler);
    QCoreApplication a(argc, argv);

    Configurator config;
    CloProxy cloud(&config);
    QVector<DevProxy*> devices(config.devicesAmount);

    for (int i = 0; i < config.devicesAmount; i++) {
        DevProxy *device = new DevProxy(&config);
        device->connectCloud(&cloud);
        cloud.connectDev(device);
        devices[i] = device;
    }

    qDebug() << "running..";
    return a.exec();


    // For Modbus tests:

    //Modbus* m = (Modbus*) devices[1]->ioDevice;
    //QVector<Message>* messages = new QVector<Message>();
    //Message* mesg = new Message("\x02\x03", QString::fromAscii("\x00\x07\x00\x04", 4));
    //Message* mesg2 = new Message("\x02\x03", QString::fromAscii("\x00\x07\x00\x07", 4));
    //Message* mesg3 = new Message("\x02\x03", QString::fromAscii("\x00\x2A\x00\x08", 4));
    //Message* mesg4 = new Message("\x02\x03", QString::fromAscii("\x00\x05\x00\x07", 4));
    //Message* mesg5 = new Message("\x02\x03", QString::fromAscii("\x00\x03\x00\x08", 4));
    //Message* mesg6 = new Message("\x02\x03", QString::fromAscii("\x00\x05\x00\x07", 4));
    //Message* mesg7 = new Message("\x02\x03", QString::fromAscii("\x00\x2A\x00\x08", 4));
    //messages->append(*mesg);
    //messages->append(*mesg2);
    //messages->append(*mesg3);
    //messages->append(*mesg4);
    //messages->append(*mesg5);
    //messages->append(*mesg6);
    //messages->append(*mesg7);
    //m->write(*messages);
    //m->readFromSensor();
    //m->Test();
    //m->inmediateWrite();
    //QString s = QString::fromAscii("\x00\x0C", 2);
    //qDebug() << (int) s.at(1).toAscii();
    //qDebug() << "running..";
    //return a.exec();

}
