#include "configurator.h"

#include "Cloud/cloconnection.h"
#include "Sensors/devconnection.h"
#include "Cloud/Mock/mockcloud.h"
#include "Sensors/Mock/mockdev.h"
#include "Sensors/SNG/sngconnection.h"
#include "Sensors/Topology/topologysensor.h"
#include "Sensors/Modbus/modbus.h"
#include "Cloud/Cosm/cosmcloud.h"
#include "Cloud/TopologyCloud/topologycloud.h"

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

    root = doc.documentElement();
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

            if (cloudType == "cosm") {
                sendFeed = e.attribute("sendFeed", "");
                receiveFeed = e.attribute("receiveFeed", "");
                apiKey = e.attribute("apikey", "");
            }

            if (cloudType == "topology") {
                cloudPort = e.attribute("port", "").toInt();
                cloudAddress = e.attribute("address", "");
            }
        }
    }

    // Count devices
    e = root.firstChildElement("device");
    devicesAmount = 0;
    while (!e.isNull()) {
        devicesAmount += 1;
        e = e.nextSiblingElement("device");
    }

    if (devicesAmount == 0) {
        qCritical("Malformed config file - can't read device");
    }

    // This pointer will be used later when creating all the devices
    devPointer = root.firstChildElement("device");
    devNo = 0;

    devDicts.resize(devicesAmount);
}

CloConnection* Configurator::giveCloud() { 
    CloConnection* cloud;

    if (cloudType == "cosm") {
        cloud = new CosmCloud(this);
    } else if (cloudType == "topology") {
        cloud = new TopologyCloud(this);
    }
    else {
        qWarning("Using mock cloud..");
        cloud = new MockCloud();
    }
    return cloud->create(this);
}

DevConnection* Configurator::readDevice(QDomElement &devPointer) {
  //     qDebug() << "Attempting device read from" << devPointer.nodeName();
    DevConnection *device;
    QDomElement e = devPointer;
    QString devType = e.attribute("type", "");
    QString devName = e.attribute("name", "");
    QDomElement m = e.firstChildElement("mappings");

    if (!m.isNull()) {
        QDomNamedNodeMap mappings = m.attributes();

        for (int i = 0; i < mappings.size(); i++) {
            QDomNode nd = mappings.item(i);
 //         qDebug() << devNo << nd.nodeName() << nd.nodeValue();
            devDicts[devNo].insert(nd.nodeName(), nd.nodeValue());
        }
    }

    devNamesToNumbers[devName] = devNo;
    devNumbersToNames[devNo] = devName;


    // Create device
    if (devType == "sng") {
        device = new SngConnection();
    } else if (devType == "topology") {
        device = new TopologySensor();
    } else if (devType == "modbus") {
        device = new Modbus(this, devNo);
    } else {
        qWarning("Using mock device");
        device = new MockDev();
    }


    // Put key-mappings-stuff into dictionary


    devPtrs.push_back(device);
    return device;
}

DevConnection* Configurator::giveDevice() {
    if (devPointer.isNull()) {
        qWarning() << "Configurator: no more devices to read.";
    }
    DevConnection *device = readDevice(devPointer);

    devNo += 1;
    devPointer = devPointer.nextSiblingElement();

    return device->create(this, devNo - 1);
}


QString Configurator::deviceTranslate(int no, QString key) {
    if (!devDicts[no].contains(key)) {
        qWarning() << "Device number: " << no << " asked for non-existent key: " << key;
        return "";
    } else {
        return devDicts[no][key];
    }
}

QString Configurator::getReceiveFeed() {
    return receiveFeed;
}

QString Configurator::getSendFeed() {
    return sendFeed;
}

QString Configurator::getCloudAddress() {
    return cloudAddress;
}

int Configurator::getCloudPort() {
    return cloudPort;
}


QString Configurator::getFeed() {
    return feedNo;
}

QString Configurator::getApiKey() {
    return apiKey;
}

// Do usuniecia
QString Configurator::getDest()
{
    return "2.3.4";
}

// Do usuneicia
QString Configurator::getFrameType()
{
    return "Value";
}
