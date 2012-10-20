#include "SensorFactory.h"
#include "Sensors/Sensor.h"


SensorFactory* SensorFactory::m_instance = NULL;

SensorFactory* SensorFactory::instance()
{
    if (m_instance == NULL)
    {
        m_instance = new SensorFactory();
    }

    return m_instance;
}

Sensor* SensorFactory::newObject (QString sensorType, KeyValueMap &config)
{
    return new Sensor();
}
