#include "sngconnection.h"

const char * heartBeatFrame = "\xff\xff\x00\x00\x00\xff\xff\xff\xff\x00\x00\x00\x00\xfe";
const int heartBeatInterval = 10;

SngConnection::SngConnection()
{
}

SngConnection::SngConnection(Configurator *config, int n) : heartBeatNo(0), no(n)
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

void SngConnection::write(QVector<MessageSample> messages)
{
    for (int i = 0; i < messages.size(); i++)
    {
        sendMessage(messages[i]);
    }
}

QVector<MessageSample> SngConnection::readAll()
{
    QVector<MessageSample> res = QVector<MessageSample>(msgQue);
    msgQue.clear();
    return res;
}

void SngConnection::readFromSensor()
{
    char msg[SNG_FRAME_SIZE];
    int errNo = commServer->read(msg, SNG_FRAME_SIZE);

    if (errNo != 14)
    {
        qDebug() << "SngConnection::readFromSensor(): wrong frame size; expected 14 got" << errNo << " bytes";
        return;
    }

    if (checkHeartBeat(msg))
    {
        return;
    }

    qDebug() << "SngConnection::readFromSensor(): read " << errNo << " bytes";

    SngFrame frame;

    if (msgParser.parseMsg(msg, frame))
    {
        qDebug() << "SngConnection::readFromSensor(): error in parsing; dropping message: " << printMsg(msg);
        return;
    }

    qDebug() << "SngConnection::readFromSensor(): parsing frame OK";

    MessageSample m;
    if (translateFrameToMessage(frame, m))
    {
        qDebug() << "SngConnection::readFromSensor(): no configuration for frame, dropped: " << frame.toString();
        return;
    }

    msgQue.push_back(m);

    qDebug() << "SngConnection::readFromSensor(): message ("
             << msgQue[msgQue.size()-1].key << ", " << msgQue[msgQue.size()-1].value
             << ") ready for cloud";

    if (msgQue.size() > 0)
        emit readyToRead();
}

void SngConnection::handleError(QAbstractSocket::SocketError error)
{
    qWarning() << "Error with CommServer: " << error;
}

void SngConnection::sendMessage(MessageSample& msg)
{
    SngFrame frame;
    if (translateMessageToFrame(msg, frame))
    {
        qDebug() << "SngConnection::sendMessage: cannot translate msg to frame; dropping msg ("
                 << msg.key << ", " << msg.value << ")";
        return;
    }

    char msgToSend[14];

    msgCreator.prepareMsg(frame, msgToSend);

    qint64 dataLength = commServer->write(msgToSend, SNG_FRAME_SIZE);
    qDebug() << "SngConnection::sendMessage(): sent " << dataLength << " bytes of data to sensor";
}

bool SngConnection::translateMessageToFrame(MessageSample& msg, SngFrame& frame)
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

bool SngConnection::translateFrameToMessage(SngFrame & frame, MessageSample& msg)
{
    QString src = frame.src.toString();
    QString dest = frame.dest.toString();
    QString type = sngFrameTypeNames[frame.type];

    QString key = conf->deviceTranslate(no, "__" + src + "__" + dest + "__" + type);

    if (key.isEmpty())
    {
        key = conf->deviceTranslate(no, "__0.0.0__" + dest + "__" + type);
    }

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
        s.sprintf("%#X ", (int) ((msg[i]+256)%256));
        res += s;
    }
    return res;
}

bool SngConnection::checkHeartBeat(char *msg)
{
    for (int i = 0; i < 14; i++)
    {
        if (msg[i] != heartBeatFrame[i]) return false;
    }

    heartBeatNo++;
    if (heartBeatNo%heartBeatInterval == 0)
    {
        qDebug() << "SngConnection: got SNG >heartbeat< number" << heartBeatNo;
    }

    return true;
}
