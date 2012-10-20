#include "CloudFactory.h"
#include "Cloud/Cloud.h"


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
    return new Cloud();
}
