
#include "SensorFactory.h"
#include "Sensors/Sensor.h"
#include "Sensors/DummySensor.h"
#include "Sensors/CommandProcessor.h"

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
    if (sensorType == "dummy")
        return new DummySensor(config);
    else if (sensorType == "command_processor")
        return new CommandProcessor(config);

    return NULL;
}
