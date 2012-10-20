
#include <QDebug>
#include <QStringList>

#include "Configurator2.h"
#include "Cloud/Cloud.h"
#include "Sensors/Sensor.h"


Configurator2::Configurator2(
        QString configFileName,
        AbstractCloudFactory* cloudFactory,
        AbstractSensorFactory* sensorFactory):
    m_cloudFactory(cloudFactory), m_sensorFactory(sensorFactory)
{
    m_settings = new QSettings(configFileName, QSettings::IniFormat);
    qDebug() << "Configuring with: " + m_settings->fileName();
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
        (*map)[key] = settings.value(key).toString();
    }

    return map;
}


void Configurator2::configure()
{
    KeyValueMap *config = NULL;

    // create clouds
    int size = m_settings->beginReadArray("clouds");
    for (int i = 0; i < size; i++)
    {
        m_settings->setArrayIndex(i);
        config = getKeyValueMap(*m_settings);
        m_clouds.push_back( m_cloudFactory->newObject((*config)["type"], *config) );
        delete config;
    }
    m_settings->endArray();

    // create sensors
    size = m_settings->beginReadArray("sensors");
    for (int i = 0; i < size; i++)
    {
        m_settings->setArrayIndex(i);
        config = getKeyValueMap(*m_settings);
        m_sensors.push_back( m_sensorFactory->newObject((*config)["type"], *config) );
        delete config;
    }
    m_settings->endArray();

    // connect clouds and sensors
}
