
#include "Sensor.h"

Sensor::Sensor(QObject *parent) :
    QObject(parent)
{
    m_connected = false;
}

Sensor::~Sensor()
{

}

