#ifndef CONFIGURATOR2_H
#define CONFIGURATOR2_H

#include <QObject>
#include <QSettings>

#include "KeyValueMap.h"
#include "CloudFactory.h"
#include "SensorFactory.h"

class Configurator2 : public QObject
{
    Q_OBJECT

public:

    Configurator2(
        QString configFileName,
        AbstractCloudFactory* cloudFactory,
        AbstractSensorFactory* sensorFactory
    );

    ~Configurator2();

    KeyValueMap *getKeyValueMapForGroup(QString group);
    int configureCloudsAndSensors();

private:
    static KeyValueMap* getKeyValueMap(const QSettings &settings);

    QSettings *m_settings;
    AbstractCloudFactory *m_cloudFactory;
    AbstractSensorFactory *m_sensorFactory;

    QHash<QString, Cloud*> m_clouds;
    QHash<QString, Sensor*> m_sensors;
};

#endif // CONFIGURATOR2_H
