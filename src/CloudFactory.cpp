#include "CloudFactory.h"

#include "Cloud/Cloud.h"
#include "Cloud/DummyCloud.h"
#include "Cloud/sngconnect/SNGConnectCloud.h"

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
    Cloud *cloud = NULL;

    if (cloudType == "dummy") {
        cloud = new DummyCloud(config);
    }
    else if (cloudType == "sngconnect") {
        cloud = new SNGConnectCloud(config);
    }

    if (cloud && !cloud->initialised()) {
        delete cloud;
        cloud = NULL;
    }

    return cloud;
}
