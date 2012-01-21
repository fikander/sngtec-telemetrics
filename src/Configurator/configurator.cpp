#include "configurator.h"

#include "Cloud/cloconnection.h"
#include "Sensors/devconnection.h"
#include "Cloud/Mock/mockcloud.h"
#include "Sensors/Mock/mockdev.h"
#include "Sensors/SNG/sngconnection.h"
#include "Sensors/Modbus/modbus.h"
#include "Cloud/Pachube/pachubecloud.h"

#include "Message/message.h"
#include "Configurator/configurator.h"


Configurator::Configurator() {
    QDomDocument doc("TelemetronConfig");
    QFile file("config.xml");

    if (!file.open(QIODevice::ReadOnly))
      qCritical() << "Error reading config";

    if (!doc.setContent(&file)) {
      file.close();
      qCritical() << "Error reading config";
    }

    file.close();

    QDomElement root = doc.documentElement();
    QDomNode n;
    QDomElement e;

    // Read cloud
    n = root.firstChildElement("cloud");
    if (n.isNull()) {
        qCritical("Malformed config file - can't read cloud");
    } else {
        e = n.toElement();

        if (e.tagName() == "cloud") {
            cloudType = e.attribute("type", "");

            if (cloudType == "pachube") {
                feedNo = e.attribute("feedno", "");
                apiKey = e.attribute("apikey", "");
            }
        }
    }
    // Read dev
    n = root.firstChildElement("device");
    if (n.isNull()) {
        qCritical("Malformed config file - can't read device");
    } else {
        e = n.toElement();

        if (e.tagName() == "device") {
            devType = e.attribute("device", "");
        }
    }
}

CloConnection* Configurator::giveCloud() { 
    CloConnection* cloud;

    if (cloudType == "pachube")
        cloud = new PachubeCloud(this);
    else {
        qWarning("Using mock cloud..");
        cloud = new MockCloud();
    }
    return cloud->create(this);
}

DevConnection* Configurator::giveDevice() {
    DevConnection *device;

    if (devType == "sng") {
        device = new SngConnection();
    } else if (devType == "modbus") {
        device = new Modbus(this);
    } else {
        qWarning("Using mock device");
        device = new MockDev();
    }

    return device->create(this);
}

QString Configurator::getFeed() {
    return feedNo;
}

QString Configurator::getApiKey() {
    return apiKey;
}

QString Configurator::getDest()
{
    return "2.3.4";
}

QString Configurator::getFrameType()
{
    return "Value";
}
