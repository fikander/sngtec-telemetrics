#ifndef CLOUDFACTORY_H
#define CLOUDFACTORY_H

#include "KeyValueMap.h"
#include "Cloud/Cloud.h"

class AbstractCloudFactory
{
public:
    virtual Cloud* newObject (QString cloudType, KeyValueMap& config) = 0;
    virtual ~AbstractCloudFactory() {}
};

class CloudFactory: public AbstractCloudFactory
{
public:
    static CloudFactory* instance();
    virtual Cloud* newObject (QString cloudType, KeyValueMap& config);
protected:
    CloudFactory() {}
private:
    static CloudFactory *m_instance;
};

#endif // CLOUDFACTORY_H
