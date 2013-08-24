

#include <QStringList>

#include "debug.h"
#include "Configurator2.h"
#include "Cloud/Cloud.h"
#include "Sensors/Sensor.h"


Configurator2::Configurator2(QString configFileName,
        AbstractCloudFactory* cloudFactory,
        AbstractSensorFactory* sensorFactory):
    m_cloudFactory(cloudFactory), m_sensorFactory(sensorFactory)
{
    m_settings = new QSettings(configFileName, QSettings::IniFormat);
    QDEBUG << "Configuring with: " + m_settings->fileName();
}


Configurator2::~Configurator2()
{
    if(!m_clouds.isEmpty())
    {
        qDeleteAll(m_clouds);
        m_clouds.clear();
    }

    if(!m_sensors.isEmpty())
    {
        qDeleteAll(m_sensors);
        m_sensors.clear();
    }
}


KeyValueMap* Configurator2::getKeyValueMap(const QSettings &settings)
{
    KeyValueMap *map = new KeyValueMap();

    QStringList children = settings.childKeys();
    foreach(QString key, children)
    {
        (*map)[key] = settings.value(key);
    }

    return map;
}

KeyValueMap *Configurator2::getKeyValueMapForGroup(QString group)
{
    m_settings->beginGroup(group);
    KeyValueMap *map = getKeyValueMap(*m_settings);
    m_settings->endGroup();

    return map;
}

int Configurator2::configureCloudsAndSensors()
{
    KeyValueMap *config = NULL;
    int connectedEntities=0;

    // create clouds based on settings
    int size = m_settings->beginReadArray("clouds");
    for (int i = 0; i < size; i++) {
        m_settings->setArrayIndex(i);
        config = getKeyValueMap(*m_settings);
        Cloud *cloud = m_cloudFactory->newObject(
            (*config)["type"].toString(), *config
        );
        if (cloud != NULL) {
            if (m_clouds.contains(cloud->getName())) {
                QWARNING << "Duplicate Cloud name, ignoring: " << cloud->getName();
            } else {
                m_clouds[cloud->getName()] = cloud;
            }
        } else {
            QWARNING << "Unrecognised or misconfigured cloud of type: " <<
                (*config)["type"].toString();
        }
        delete config;
    }
    m_settings->endArray();


    // create sensors based on settings
    size = m_settings->beginReadArray("sensors");
    for (int i = 0; i < size; i++) {
        m_settings->setArrayIndex(i);
        config = getKeyValueMap(*m_settings);
        Sensor *sensor = m_sensorFactory->newObject(
            (*config)["type"].toString(), *config
        );
        if (sensor != NULL) {
            if (m_sensors.contains(sensor->getName())) {
                QWARNING << "Duplicate Sensor name, ignoring: " << sensor->getName();
            } else {
                m_sensors[sensor->getName()] = sensor;
            }
        } else {
            QWARNING << "Unrecognised of misconfigured sensor of type: " <<
                (*config)["type"].toString();
        }
        delete config;
    }
    m_settings->endArray();

    //analyse mapping of sensors to clouds
    size = m_settings->beginReadArray("mapping");
    for (int i = 0; i < size; i++) {
        m_settings->setArrayIndex(i);
        config = getKeyValueMap(*m_settings);
        QString mapping = config->value("mapping").toString();

        QString sensorName = mapping.section(':', 0, 0);
        QString cloudName = mapping.section(':', 1, 1);
        if (m_sensors.contains(sensorName) && m_clouds.contains(cloudName)) {
            Sensor *sensor = m_sensors[sensorName];
            Cloud *cloud = m_clouds[cloudName];
            QObject::connect(
                sensor, SIGNAL(received(QSharedPointer<Message>)),
                cloud, SLOT(send(QSharedPointer<Message>)));
            QObject::connect(
                cloud, SIGNAL(received(QSharedPointer<Message>)),
                sensor, SLOT(send(QSharedPointer<Message>)));

            if (!sensor->connected()) {
                sensor->connect();
            }
            if (!cloud->connected()) {
                cloud->connect();
            }
            QDEBUG << "Connected sensor " << sensor->getName() <<
                      " to cloud " << cloud->getName();
        } else {
            QWARNING << "Sensor or cloud not defined: " << mapping;
        }
    }
    foreach(Sensor *sensor, m_sensors) {
        connectedEntities += sensor->connected() ? 1 : 0;
    }
    foreach(Cloud *cloud, m_clouds) {
        connectedEntities += cloud->connected() ? 1 : 0;
    }

    return connectedEntities;
}
