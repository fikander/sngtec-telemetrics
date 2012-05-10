#include "sngconnection.h"

SngConnection::SngConnection()
{
}

SngConnection::SngConnection(Configurator *config, int n) : no(n)
{
    this->conf = config;
    commServer = new QTcpSocket(this);

    this->commServerHostName = conf->deviceTranslate(no, addresKey);
    this->port = conf->deviceTranslate(no, portKey).toInt();
    this->physicalAddress.make(conf->deviceTranslate(no, physicalAddressKey));

    connect(commServer, SIGNAL(readyRead()), this, SLOT(readFromSensor()));
    connect(commServer, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleError(QAbstractSocket::SocketError)));

    commServer->connectToHost(commServerHostName, port);
}

SngConnection::~SngConnection()
{
}

DevConnection* SngConnection::create(Configurator *config, int no)
{
    return new SngConnection(config, no);
}

DevConnection* SngConnection::clone(Configurator *config, int no)
{
    return new SngConnection(config, no);
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
    qDebug() << "SngConnection::readFromSensor(): read from sensor\n";
    char msg[SNG_FRAME_SIZE];
    int errNo = commServer->read(msg, SNG_FRAME_SIZE);
    qDebug() << "SngConnection::readFromSensor(): read " << errNo << " bytes\n";

    SngFrame frame;

    if (msgParser.parseMsg(msg, frame))
    {
        qDebug() << "SngConnection::readFromSensor(): error in parsing; dropping message: " << printMsg(msg);
        return;
    }

    qDebug() << "SngConnection::readFromSensor(): parsing frame OK\n";

    Message m;
    if (translateFrameToMessage(frame, m))
    {
        qDebug() << "SngConnection::readFromSensor(): no configuration for frame, dropped: " << frame.toString();
        return;
    }

    msgQue.push_back(m);

    qDebug() << "SngConn::readFromSensor(): get message ("
             << msgQue[msgQue.size()-1].key << ", " << msgQue[msgQue.size()-1].value
             << ")\n";

    emit readyToRead();
}

void SngConnection::handleError(QAbstractSocket::SocketError error)
{
    qWarning() << "Error with CommServer: " << error;
}

void SngConnection::sendMessage(Message& msg)
{
    SngFrame frame;
    if (translateMessageToFrame(msg, frame))
    {
        qDebug() << "SngConnection::sendMessage: cannot translate msg to frame; dropping msg ("
                 << msg.key << ", " << msg.value << ")\n";
        return;
    }

    char msgToSend[14];

    msgCreator.prepareMsg(frame, msgToSend);

    qint64 dataLength = commServer->write(msgToSend, SNG_FRAME_SIZE);
    qDebug() << "SngConnection::sendMessage(): sent " << dataLength << " bytes of data to sensor" << "\n";
}

bool SngConnection::translateMessageToFrame(Message& msg, SngFrame& frame)
{
    QStringList ls = conf->deviceTranslate(no, msg.key).split(separator);

    if (ls.length() != 2)
    {
        qDebug() << "SngConnection::translateMessageToFrame: wrong configuration of device no" << no
                 << "with key: " << msg.key;
        return true;
    }
    QString addr = ls.at(0);
    QString typ = ls.at(1);

    SngFrameType frameType;
    if (parseFrameType(typ, frameType))
    {
        qDebug() << "SngConnection::translateMessageToFrame: wrong configuration of device no" << no
                 <<  ". Cannot parse frame type:" << typ;
        return true;
    }

    if (physicalAddress.isWrongAddr(addr))
    {
        qDebug() << "SngConnection::translateMessageToFrame: wrong configuration of device no" << no
                 <<  ". Cannot parse SNG address:" << addr;
        return true;
    }

    frame.src = physicalAddress;
    frame.dest = SngPhysicalAddress(addr);
    frame.type = frameType;
    frame.value = msg.value;
    return false;
}

bool SngConnection::parseFrameType(QString& s, SngFrameType& frameType)
{
    for (int i = 0; i < SNG_FRAME_TYPES_NR; i++)
    {
        if (! s.compare(sngFrameTypeNames[i]))
        {
            frameType = (SngFrameType) i;
            return false;
        }
    }
    return true;
}

bool SngConnection::translateFrameToMessage(SngFrame & frame, Message& msg)
{
    QString src = frame.src.toString();
    QString dest = frame.src.toString();
    QString type = sngFrameTypeNames[frame.type];

    QString key = conf->deviceTranslate(no, "__" + src + "__" + dest + "__" + type);

    if (key.isEmpty())
        return true;

    msg.key = key;
    msg.value = frame.value;
    return false;
}

QString SngConnection::printMsg(char *msg)
{
    QString res = "";
    for (int i = 0; i < 14; i++) {
        QString s = "";
        s.sprintf("%#X", (int) ((msg[i]+256)%256));
        res += s;
    }
    return res;
}
