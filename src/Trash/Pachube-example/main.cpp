#include <QCoreApplication>
#include "src/Cloud/Pachube/pachubecloud.h"
#include "src/Cloud/Pachube/pachubexml.h"
#include "src/Cloud/cloconnection.h"
#include "src/Message/message.h"
#include "src/Configurator/configurator.h"
#include <QTime>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    Configurator config;
    CloConnection *cloud = new PachubeCloud(config);
    CloConnection::MessagesSet mess; 
    Message message;
    message.key = "1";
    message.value = "70";
    mess.append(message);
    cloud->write(mess);
    app.exec();
    return 0;
}
