#include "sngconnection.h"

SngConnection::SngConnection()
{
}

SngConnection::SngConnection(Configurator *config) :
    commServerHostName("89.231.108.25"), port(8888),
    physicalAddress("2.4.7"), defaultDest("1.2.3")
{
    this->conf = config;
    commServer = new QTcpSocket(this);

    connect(commServer, SIGNAL(readyRead()), this, SLOT(readFromSensor()));
    connect(commServer, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleError(QAbstractSocket::SocketError)));

    commServer->connectToHost(commServerHostName, port);
}

SngConnection::~SngConnection()
{
}

DevConnection* SngConnection::create(Configurator *config)
{
    return new SngConnection(config);
}

DevConnection* SngConnection::clone(Configurator *config)
{
    return new SngConnection(config);
}

void SngConnection::write(QVector<Message> messages)
{
    for (int i = 0; i < messages.size(); i++)
    {
        sendMessage(messages[i]);
    }
}

QVector<Message> SngConnection::readAll()
{
    QVector<Message> res = QVector<Message>(msgQue);
    msgQue.clear();
    return res;
}

void SngConnection::readFromSensor()
{
    qDebug() << "read from sensor\n";
    char msg[SNG_FRAME_SIZE];
    int errNo = commServer->read(msg, SNG_FRAME_SIZE);
    qDebug() << "read " << errNo << " bytes\n";

    SngFrame frame;

    if (msgParser.parseMsg(msg, frame))
    {
        qDebug() << "SngConn::readFromSensor(): error in parsing\n";
        return;
    }

    qDebug() << "SngConn::readFromSensor(): parsing OK\n";

    msgQue.push_back(translateFrameToMessage(frame));

    qDebug() << "SngConn::readFromSensor(): get message ("
             << msgQue[msgQue.size()-1].key << ", " << msgQue[msgQue.size()-1].value
             << ")\n";

    emit readyToRead();
}

void SngConnection::handleError(QAbstractSocket::SocketError error)
{
    qDebug() << "Error with CommServer: " << error;
}

void SngConnection::sendMessage(Message& msg)
{
    SngFrame frame = translateMessageToFrame(msg);
    char msgToSend[14];

    msgCreator.prepareMsg(frame, msgToSend);

    qint64 dataLength = commServer->write(msgToSend, SNG_FRAME_SIZE);
    qDebug() << "sent " << dataLength << " bytes of data to sensor" << "\n";
}

SngFrame SngConnection::translateMessageToFrame(Message& msg)
{
    //should ask config about what to
//    QString frameTypeName = conf->getFrameType();
    SngFrameType frameType = parseFrameType(msg.key);
    return SngFrame(physicalAddress, conf->getDest(), frameType, msg.value);
}

SngFrameType SngConnection::parseFrameType(QString& s)
{
    for (int i = 0; i < SNG_FRAME_TYPES_NR; i++)
    {
        if (! s.compare(sngFrameTypeNames[i]))
            return (SngFrameType) i;
    }
    qDebug() << "SngConnection::parseFrameType : wrong Frame Type";
    return SngValue;
}

Message SngConnection::translateFrameToMessage(SngFrame & frame)
{
    return Message(sngFrameTypeNames[frame.type], frame.value);
}

