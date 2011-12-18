#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QObject>

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

private:
    QString feedNo;
    QString apiKey;
};

#endif // CONFIGURATOR_H
