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
};

#endif // CONFIGURATOR_H
