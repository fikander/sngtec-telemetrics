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

    int devicesAmount;
    QString deviceTranslate(int no, QString msg);

    //SNG - narazie stuby
    QString getDest();
    QString getFrameType();

    QHash<QString, int> devNamesToNumbers;
    QHash<int, QString> devNumbersToNames;
    QVector<DevConnection*> devPtrs;

private:
    QDomElement root;
    DevConnection* readDevice(QDomElement &devPointer);
    QVector<QHash<QString, QString> > devDicts;
    QString feedNo;
    QString apiKey;
    QString cloudType, devType;
    QDomElement devPointer;
    int devNo;
};

#endif // CONFIGURATOR_H
