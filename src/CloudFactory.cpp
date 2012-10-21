#include "CloudFactory.h"

#include "Cloud/Cloud.h"
#include "Cloud/DummyCloud.h"

CloudFactory* CloudFactory::m_instance = NULL;

CloudFactory* CloudFactory::instance()
{
    if (m_instance == NULL)
    {
        m_instance = new CloudFactory();
    }

    return m_instance;
}

Cloud* CloudFactory::newObject (QString cloudType, KeyValueMap &config)
{
    if (cloudType == "dummy")
        return new DummyCloud(config);

    return NULL;
}
