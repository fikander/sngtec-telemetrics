
#include <QCoreApplication>
#include <QSettings>

#include "Logger/logger.h"

#include "Configurator2.h"
#include "CloudFactory.h"
#include "SensorFactory.h"

//class A
//{
//public:
//    ~A() {qDebug() << "delete";}
//};

//void test()
//{
//    QSharedPointer<A> y;
//    QList< QSharedPointer<A> > list;

//    {
//        QSharedPointer<A> x = QSharedPointer<A>(new A());
//        y = x;
//        list.append(x);

//        x.clear();
//        qDebug()<<"1";
//    }
//    y.clear();
//    qDebug()<<"2";

//    list.clear();
//    qDebug()<<"3";
//}

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    CloudFactory *cloudFactory = CloudFactory::instance();
    SensorFactory *sensorFactory = SensorFactory::instance();

    Configurator2 configurator("telemetron.ini", cloudFactory, sensorFactory);

    // set up logging
    KeyValueMap *config = configurator.getKeyValueMapForGroup("logging");
    Logger::initialise(*config);
    delete config;

    qInstallMsgHandler(Logger::getInstance()->loggingHandler);

    // set up clouds and sensors
    configurator.configureCloudsAndSensors();

    delete cloudFactory;
    delete sensorFactory;

/*
    Configurator config;
    CloProxy cloud(&config);
    QVector<DevProxy*> devices(config.devicesAmount);

    for (int i = 0; i < config.devicesAmount; i++) {
        DevProxy *device = new DevProxy(&config);
        device->connectCloud(&cloud);
        cloud.connectDev(device);
        devices[i] = device;
    }
*/

    qDebug() << "running..";
    return a.exec();

    // For Modbus tests:

    //Modbus* m = (Modbus*) devices[0]->ioDevice;
    //QVector<Message>* messages = new QVector<Message>();
    //Message* mesg = new Message(QString("readFromSecond"), QString("dataFromSecond"));
    //Message* mesg2 = new Message(QString("readFromSecond"), QString("windRegisters"));
    //Message* mesg3 = new Message(QString("readFromSecond"), QString("windRegisters"));
    //Message* mesg4 = new Message(QString("readFromSecond"), QString("stateRegisters"));
    //Message* mesg5 = new Message(QString("readFromSecond"), QString("constRegisters"));
    //Message* mesg6 = new Message(QString("\x02\x03"), QString::fromAscii("\x00\x03\x00\x08", 4));
    //Message* mesg7 = new Message(QString("\x02\x03"), QString::fromAscii("\x00\x07\x00\x07", 4));
    //messages->append(*mesg);
    //messages->append(*mesg2);
    //messages->append(*mesg3);
    //messages->append(*mesg4);
    //messages->append(*mesg5);
    //messages->append(*mesg6);
    //messages->append(*mesg7);
    //m->write(*messages);
    //m->readFromSensor();
    //qDebug() << "running..";
    //return a.exec();

}
