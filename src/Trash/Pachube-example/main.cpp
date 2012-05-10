#include <QCoreApplication>
#include "src/Cloud/Cosm/cosmcloud.h"
#include "src/Cloud/Cosm/cosmxml.h"
#include "src/Cloud/cloconnection.h"
#include "src/Message/message.h"
#include "src/Configurator/configurator.h"
#include <QTime>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    Configurator config;
    CloConnection *cloud = new CosmCloud(&config);
    CloConnection::MessagesSet mess; 
    Message message;
    message.key = "1";
    message.value = "70";
    mess.append(message);
    cloud->write(mess);
    app.exec();
    return 0;
}
