#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QDomDocument>
#include <QString>
#include <QFile>
#include <QDomElement>

class DevConnection;
class CloConnection;

class Configurator : public QObject
{
public:
    Configurator();
    DevConnection* giveDevice();
    CloConnection* giveCloud();
    QString getFeed();
    QString getApiKey();

    //SNG - narazie stuby
    QString getDest();
    QString getFrameType();

private:
    QString feedNo;
    QString apiKey;
    QString cloudType, devType;

};

#endif // CONFIGURATOR_H
