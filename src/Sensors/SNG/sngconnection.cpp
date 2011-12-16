#include "sngconnection.h"

SNGconnection::SNGconnection(Configurator & config)
{
    this->conf = &config;
}

SNGconnection::~SNGconnection()
{
}

DevConnection* SNGconnection::create(Configurator &config)
{
    return new SNGconnection(config);
}

DevConnection* SNGconnection::clone(Configurator &config)
{
    return new SNGconnection(config);
}

void SNGconnection::write(QVector<Message> messages)
{
    //it = messages.constBegin();
}

QVector<Message> SNGconnection::readAll() {
    QVector<Message> res;
    return res;
}




