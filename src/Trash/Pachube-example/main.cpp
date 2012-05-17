#include <QCoreApplication>
#include "Cloud/Cosm/cosmcloud.h"
#include "Cloud/Cosm/cosmxml.h"
#include "Cloud/cloconnection.h"
#include "Message/message.h"
#include "Configurator/configurator.h"
#include <QTime>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    CloConnection *cloud = new CosmCloud( "49041" , "49041" , "KGt1d_erNyuyZ1ObC4TOMEIZMlKNQSoWRUfMjKn5Vh0");
    CloConnection::MessagesSet mess; 
    Message message;
    message.key = "key5";
    message.value = "70";
    mess.append(message);
    cloud->write(mess);
    app.exec();
    return 0;
}
