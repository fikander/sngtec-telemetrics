#include "configurator.h"

#include "src/Cloud/cloconnection.h"
#include "src/Sensors/devconnection.h"
#include "src/Cloud/Mock/mockcloud.h"
#include "src/Sensors/Mock/mockdev.h"
#include "src/Sensors/SNG/sngconnection.h"
#include "src/Cloud/Pachube/pachubecloud.h"

#include "src/Message/message.h"
#include "src/Configurator/configurator.h"


Configurator::Configurator() {
    feedNo = "42213";
    apiKey = "7p1iGBicjYRQS20cEDjFdr3SnTldEyq-kL2XabP6iXw";
}

CloConnection* Configurator::giveCloud() { 
    PachubeCloud cloud(this);
    return cloud.create(this);
}

DevConnection* Configurator::giveDevice() {
    MockDev device;
    return device.create(this);
}

QString Configurator::getFeed() {
    return feedNo;
}

QString Configurator::getApiKey() {
    return apiKey;
}
