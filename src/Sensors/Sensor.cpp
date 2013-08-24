#include "debug.h"
#include "Sensor.h"

Sensor::Sensor(KeyValueMap &config, QObject *parent) :
    QObject(parent), m_connected(false)
{
    if (config.contains("name")) {
        name = config["name"].toString();
        m_initialised = true;
        QDEBUG << "New Sensor definition: " << name;
    } else {
        QERROR << "Name is required.";
        m_initialised = false;
    }
}

Sensor::~Sensor()
{

}

