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
/*
    Modbus* m = (Modbus*) devices[0]->ioDevice;
    QVector<Message>* messages = new QVector<Message>();
    Message* mesg = new Message(QString("ReadFromSecond"), QString("TemperatureRegisters"));
    Message* mesg2 = new Message(QString("ReadFromSecond"), QString("WindRegisters"));
    Message* mesg3 = new Message(QString("ReadFromSecond"), QString("WindRegisters"));
    Message* mesg4 = new Message(QString("ReadFromSecond"), QString("StateRegisters"));
    Message* mesg5 = new Message(QString("ReadFromSecond"), QString("ConstRegisters"));
    Message* mesg6 = new Message(QString("ReadFromSecond"), QString("SteamRegisters"));
    Message* mesg7 = new Message(QString("ReadFromSecond"), QString("PressureRegisters"));
    messages->append(*mesg);
    messages->append(*mesg2);
    messages->append(*mesg3);
    messages->append(*mesg4);
    messages->append(*mesg5);
    messages->append(*mesg6);
    messages->append(*mesg7);
    m->write(*messages);
    //m->readFromSensor();
    qDebug() << "running..";
    return a.exec();
*/
}
