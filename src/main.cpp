
#include <QCoreApplication>
#include <QSettings>
#include <iostream>

#include "debug.h"
#include "Logger/logger.h"

#include "Configurator2.h"
#include "CloudFactory.h"
#include "SensorFactory.h"

//class A
//{
//public:
//    ~A() {qDebug() << "delete";}
//    void aMethod() { qDebug() << "a method"; }
//};

//class AProxy
//{
//public:
//    AProxy(QSharedPointer<A> a): a(a) {}
//    ~AProxy() {qDebug() << "proxy delete";}
//    void proxyMethod() { qDebug() << "proxy method"; }
//    QSharedPointer<A> operator->() const
//        { return a; }
//    QSharedPointer<A> &object()
//        { return a; }

//protected:
//    QSharedPointer<A> a;
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

//    qDebug()<<"3 -----";
//    // proxy in a list
//    {
//        AProxy proxy(QSharedPointer<A>(new A()));
//        QList<AProxy> list2;
//        proxy.proxyMethod();
//        proxy->aMethod();
//        qDebug() << "3a";
//        list2.append(proxy); //new proxy object created - copied from proxy
//        list2.clear(); // copy of proxy delete, original proxy and object A stay
//        qDebug() << "3b";
//    }
//    // proxy pointer in a list
//    qDebug() << "4 -----";
//    {
//        AProxy *proxyPtr = new AProxy(QSharedPointer<A>(new A()));
//        QList<AProxy*> list2;
//        proxyPtr->proxyMethod();
//        (*proxyPtr)->aMethod();
//        proxyPtr->object().staticCast<A>();
//        qDebug() << "4a";
//        list2.append(proxyPtr);
//        delete list2.first();
//        qDebug() << "4b";
//        list2.clear();
//        qDebug() << "4c";
//    }
//    // proxy shared pointer in a list
//    qDebug() << "5 -----";
//    {
//        QSharedPointer<AProxy> proxy = QSharedPointer<AProxy>(new AProxy(QSharedPointer<A>(new A())));
//        QList<QSharedPointer<AProxy> > list2;
//        proxy->proxyMethod();
//        (*proxy)->aMethod();
//        qDebug() << "5a";
//        list2.append(proxy);
//        list2.removeFirst();
//        qDebug() << "5b";
//        list2.clear();
//        qDebug() << "5c";
//    }
//}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    CloudFactory *cloudFactory = CloudFactory::instance();
    SensorFactory *sensorFactory = SensorFactory::instance();

    Configurator2 configurator("telemetron.ini", cloudFactory, sensorFactory);

    // set up logging
    KeyValueMap *config = configurator.getKeyValueMapForGroup("logging");
    Logger::initialise(*config);
    delete config;

    qInstallMsgHandler(Logger::getInstance()->loggingHandler);

    // set up clouds and sensors
    int connected = configurator.configureCloudsAndSensors();

    delete cloudFactory;
    delete sensorFactory;

    if (connected > 0) {
        std::cout << "Connected " << connected << " entities" << std::endl;
        return app.exec();
    } else {
        std::cerr << "Nothing connected, exiting..." << std::endl;
        return 1;
    }

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
