#include "debug.h"
#include "Cloud.h"

Cloud::Cloud(KeyValueMap &config) :
    m_connected(false)
{
    if (config.contains("name")) {
        name = config["name"].toString();
        m_initialised = true;
        QDEBUG << "New Cloud definition: " << name;
    } else {
        QERROR << "Name is required.";
        m_initialised = false;
    }
}

Cloud::~Cloud()
{

}

