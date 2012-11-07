#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>
#include <QDomDocument>
#include <QString>
#include <QFile>
#include <QDomElement>
#include <QDomNode>
#include <QHash>
#include <QVector>
#include <QDebug>
//#include "Sensors/devconnection.h"

class DevConnection;
class CloConnection;

class Configurator : public QObject
{
public:
    Configurator();
    DevConnection* giveDevice();
    CloConnection* giveCloud();
    QString getApiKey();
    QString getReceiveFeed();
    QString getSendFeed();
    QString getCloudAddress();
    QString getRequestTime();
    QString getCosmTimeDifference();
    int getCloudPort();


    int devicesAmount;
    QString deviceTranslate(int no, QString msg);

    QHash<QString, int> devNamesToNumbers;
    QHash<int, QString> devNumbersToNames;
    QVector<DevConnection*> devPtrs;
    QString logPushResponse;
    QString logPushCommand;
private:
    QDomElement root;
    DevConnection* readDevice(QDomElement &devPointer);
    QVector<QHash<QString, QString> > devDicts;
    QString receiveFeed, sendFeed, cloudAddress;
    int cloudPort;
    QString apiKey;
    QString cosmTimeDifference;
    QString cosmRequestTime;
    QString cloudType, devType;
    QDomElement devPointer;
    int devNo;
};

#endif // CONFIGURATOR_H
