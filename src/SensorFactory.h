#ifndef SENSORFACTORY_H
#define SENSORFACTORY_H

#include "KeyValueMap.h"
#include "Sensors/Sensor.h"

class AbstractSensorFactory
{
public:
    virtual Sensor* newObject (QString sensorType, KeyValueMap& config) = 0;
    virtual ~AbstractSensorFactory() {}
};

class SensorFactory: public AbstractSensorFactory
{
public:
    static SensorFactory* instance();
    virtual Sensor* newObject (QString sensorType, KeyValueMap& config);
protected:
    SensorFactory() {}
private:
    static SensorFactory *m_instance;
};

#endif // SENSORFACTORY_H
