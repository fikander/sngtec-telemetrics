
#include "SensorFactory.h"
#include "Sensors/Sensor.h"
#include "Sensors/DummySensor.h"
#include "Sensors/CommandProcessor.h"
#include "Sensors/Modbus/ModbusSensor.h"

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
    Sensor *sensor = NULL;

    if (sensorType == "dummy") {
        sensor = new DummySensor(config);
    } else if (sensorType == "modbus") {
        sensor = new Modbus(config);
    } else if (sensorType == "command_processor") {
        sensor = new CommandProcessor(config);
    }

    if (sensor && !sensor->initialised()) {
        delete sensor;
        sensor = NULL;
    }

    return sensor;
}
