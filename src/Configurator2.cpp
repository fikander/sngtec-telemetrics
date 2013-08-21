

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
    // delete lists of pointers

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

void Configurator2::configureCloudsAndSensors()
{
    KeyValueMap *config = NULL;


    // create clouds based on settings
    int size = m_settings->beginReadArray("clouds");
    for (int i = 0; i < size; i++)
    {
        m_settings->setArrayIndex(i);
        config = getKeyValueMap(*m_settings);
        Cloud *cloud = m_cloudFactory->newObject(
            (*config)["type"].toString(), *config
        );
        if (cloud != NULL) {
            m_clouds.push_back( cloud );
        } else {
            QWARNING << "Unrecognised cloud type: " << (*config)["type"].toString();
        }
        delete config;
    }
    m_settings->endArray();


    // create sensors based on settings
    size = m_settings->beginReadArray("sensors");
    for (int i = 0; i < size; i++)
    {
        m_settings->setArrayIndex(i);
        config = getKeyValueMap(*m_settings);
        Sensor *sensor = m_sensorFactory->newObject(
            (*config)["type"].toString(), *config
        );
        if (sensor != NULL) {
            m_sensors.push_back( sensor );
        } else {
            QWARNING << "Unrecognised sensor type: " << (*config)["type"].toString();
        }
        delete config;
    }
    m_settings->endArray();


    // connect sensors to clouds: messages sensor -> cloud
    foreach(Sensor *sensor, m_sensors)
    {
        foreach(Cloud *cloud, m_clouds)
        {
            QObject::connect(
                sensor, SIGNAL(received(QSharedPointer<Message>)),
                cloud, SLOT(send(QSharedPointer<Message>))
            );
            QDEBUG << "Sensor received signal: " <<sensor->metaObject()->className() <<
                    " with cloud send: " << cloud->metaObject()->className();
        }
    }

    // connect clouds to sensors: messages cloud -> sensor
    // connect clouds to command processor
    foreach(Cloud *cloud, m_clouds)
    {
        foreach(Sensor *sensor, m_sensors)
        {
            QObject::connect(
                cloud, SIGNAL(received(QSharedPointer<Message>)),
                sensor, SLOT(send(QSharedPointer<Message>)));
            QDEBUG << "Cloud received signal: " << cloud->metaObject()->className() <<
                    " with sensor send: " << sensor->metaObject()->className();
        }
    }

    // connect sensors first
    foreach (Sensor *sensor, m_sensors)
    {
        sensor->connect();
    }

    // get ready with the cloud
    foreach (Cloud *cloud, m_clouds)
    {
        cloud->connect();
    }

}
