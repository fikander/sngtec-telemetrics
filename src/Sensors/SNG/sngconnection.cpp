#include "sngconnection.h"

SngConnection::SngConnection(Configurator & config) :
    commServerHostName("localhost"), port(8800),
    physicalAddress("2.4.7"), defaultDest("1.2.3")
{
    this->conf = &config;
    commServer = new QTcpSocket(this);

    connect(commServer, SIGNAL(readyRead()), this, SLOT(readFromSensor()));
    connect(commServer, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleError(QAbstractSocket::SocketError)));

    commServer->connectToHost(commServerHostName, port);
}

SngConnection::~SngConnection()
{
}

DevConnection* SngConnection::create(Configurator &config)
{
    return new SngConnection(config);
}

DevConnection* SngConnection::clone(Configurator &config)
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

}

void SngConnection::handleError(QAbstractSocket::SocketError error)
{
    qDebug() << "Error with CommServer: " << error;
}

void SngConnection::sendMessage(Message& msg)
{
    // TODO get settings from config
    sendFrame(physicalAddress, defaultDest, Value, msg.value);
}

void SngConnection::sendFrame(SngPhysicalAddress &src, SngPhysicalAddress &dest,
                              SngFrameType frameType, QString &value)
{
    char msg[14];
    setBeginAndEndOfFrame(msg);
    setFrameType(frameType, msg);
    setSrcAddr(src, msg);
    setDestAddr(dest, msg);
    setValue(frameType, msg, value);

    //TODO: parse value

    qint64 dataLength = commServer->write(msg, SNG_FRAME_SIZE);
    qDebug() << "sent " << dataLength << " bytes of data to sensor";
}

void SngConnection::setBeginAndEndOfFrame(char * buff)
{
    buff[0] = 0xFF;
    buff[1] = 0xFF;
    buff[13] = 0xFE;
}

void SngConnection::setFrameType(SngFrameType frameType, char * msg)
{
    char *buff = &msg[8];
    switch (frameType) {
    case OnOff:
        *buff = 0x01;
        break;
    case Dimm:
        *buff = 0x02;
        break;
    case Time:
        *buff = 0x03;
        break;
    case Date:
        *buff = 0x04;
        break;
    case Temp:
        *buff = 0x05;
        break;
    case Value:
        *buff = 0x06;
        break;
    default:
        break;
    }
}

void SngConnection::setSrcAddr(SngPhysicalAddress& phAddr, char *buff)
{
    setAddr(phAddr, &buff[2]);
}

void SngConnection::setDestAddr(SngPhysicalAddress& phAddr, char *buff)
{
    setAddr(phAddr, &buff[5]);
}

void SngConnection::setAddr(SngPhysicalAddress& phAddr, char *buff)
{
    buff[0] = phAddr.getByteAt(0);
    buff[1] = phAddr.getByteAt(1);
    buff[2] = phAddr.getByteAt(2);
}

void SngConnection::setValue(SngFrameType frameType, char* msg, QString& value)
{
    char *buff = &msg[9];
    buff[0] = buff[1] = buff[2] = buff[3] = 0x00;
    switch (frameType) {
    case OnOff:
        parseOnOff(buff, value);
        break;
    case Dimm:
        parseDimm(buff, value);
        break;
    case Time:
        parseTime(buff, value);
        break;
    case Date:
        parseDate(buff, value);
        break;
    case Temp:
        parseTemp(buff, value);
        break;
    case Value:
        parseValue(buff, value);
        break;
    default:
        break;
    }
}


void SngConnection::parseOnOff(char *buff, QString& value)
{
    if (value == "on")
    {
        buff[0] = 0x01;
    }
    else if (value == "off")
    {
        buff[0] = 0x00;
    }
    else
    {
        qDebug() << "SNG: Error in parsing On/Off value. Got: " << value;
    }
}


void SngConnection::parseDimm(char *buff, QString& value)
{
    if (value == "inc")
    {
        buff[0] = 0x09;
    }
    else if (value == "endInc")
    {
        buff[0] = 0x08;
    }
    else if (value == "dec")
    {
        buff[0] = 0x01;
    }
    else if (value == "endDec")
    {
        buff[0] = 0x00;
    }
    else
    {
        qDebug() << "SNG: Error in parsing Dimm value. Got: " << value;
    }
}

void SngConnection::parseTime(char *buff, QString& value)
{
    QStringList v = value.split(":");
    if (v.size() != 3)
    {
        qDebug() << "SNG: Error in parsing Time value. Got: " << value;
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            int t = v.at(i).toInt();
            if ((t > 255) || (t < 0) || ((t == 0) && (v.at(i) != "0")))
            {
                qDebug() << "SNG: Error in parsing Time value. Got: " << value;
                return;
            }
            else
            {
                buff[i] = t;
            }
        }
    }
}

//it may work not properly, because of some ambiguity in specification
void SngConnection::parseDate(char *buff, QString& value)
{
    QStringList v = value.split(".");
    if (v.size() != 3)
    {
        qDebug() << "SNG: Error in parsing Date value. Got: " << value;
    }
    else
    {
        for (int i = 0; i < 3; i++)
        {
            int t = v.at(i).toInt();
            if (t == 2)
            {
                t %= 256;
            }
            if ((t > 255) || (t < 0) || ((t == 0) && (v.at(i) != "0")))
            {
                qDebug() << "SNG: Error in parsing Date value. Got: " << value;
                return;
            }
            else
            {
                buff[i] = t;
            }
        }
    }
}

void SngConnection::parseTemp(char *buff, QString& value)
{
    QString v = value;
    if (v[0] == '-')
    {
        buff[2] = 0x01;
        v[0] = '0';
    }
    QStringList vs = v.split(".");
    if (vs.size() != 2)
    {
        qDebug() << "SNG: Error in parsing Temp value. Got: " << value;
    }
    for (int i = 0; i < 2; i++)
    {
        int  t = vs.at(i).toInt();
        if ((t == 0) && !((vs.at(i) == "0") || vs.at(i) == "00"))
        {
            qDebug() << "SNG: Error in parsing Temp value. Got: " << value;
            return;
        }
        else
        {
            buff[i] = t;
        }
    }
}

void SngConnection::parseValue(char *buff, QString& value)
{
    int v = value.toInt();
    if (((v == 0) && (value != "0")) || (v > 255) || (v < 0))
    {
        qDebug() << "SNG: Error in parsing Value value. Got: " << value;
    }
    else
    {
        buff[0] = v;
    }
}
