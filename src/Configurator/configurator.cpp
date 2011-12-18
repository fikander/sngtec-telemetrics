#include "configurator.h"

#include "src/Cloud/cloconnection.h"
#include "src/Sensors/devconnection.h"
#include "src/Cloud/Mock/mockcloud.h"
#include "src/Sensors/SNG/sngconnection.h"


Configurator::Configurator() {
}

CloConnection* Configurator::giveCloud() { 
    MockCloud m;
    return m.create(this);
}

DevConnection* Configurator::giveDevice() {
    SngConnection m;
    return m.create(this);
}
