#include "sngmsgcreator.h"
#include <QDebug>
#include <QString>
#include <QStringList>

SngMsgCreator::SngMsgCreator()
{
}

void SngMsgCreator::prepareMsg(SngFrame& frame, char * msg)
{
    setBeginAndEndOfFrame(msg);
    setFrameType(frame.type, msg);
    setSrcAddr(frame.src, msg);
    setDestAddr(frame.dest, msg);
    setValue(frame.type, msg, frame.value);
}

void SngMsgCreator::setBeginAndEndOfFrame(char * buff)
{
    buff[0] = 0xFF;
    buff[1] = 0xFF;
    buff[13] = 0xFE;
}

void SngMsgCreator::setFrameType(SngFrameType frameType, char * msg)
{
    char *buff = &msg[8];
    switch (frameType) {
    case SngOnOff:
        *buff = 0x01;
        break;
    case SngDimm:
        *buff = 0x02;
        break;
    case SngTime:
        *buff = 0x03;
        break;
    case SngDate:
        *buff = 0x04;
        break;
    case SngTemp:
        *buff = 0x05;
        break;
    case SngValue:
        *buff = 0x06;
        break;
    default:
        break;
    }
}

void SngMsgCreator::setSrcAddr(SngPhysicalAddress& phAddr, char *buff)
{
    setAddr(phAddr, &buff[2]);
}

void SngMsgCreator::setDestAddr(SngPhysicalAddress& phAddr, char *buff)
{
    setAddr(phAddr, &buff[5]);
}

void SngMsgCreator::setAddr(SngPhysicalAddress& phAddr, char *buff)
{
    for (int i = 0; i < 3; i++)
        buff[i] = phAddr.getByteAt(i);
}

void SngMsgCreator::setValue(SngFrameType frameType, char* msg, QString& value)
{
    char *buff = &msg[9];
    buff[0] = buff[1] = buff[2] = buff[3] = 0x00;
    switch (frameType) {
    case SngOnOff:
        parseOnOff(buff, value);
        break;
    case SngDimm:
        parseDimm(buff, value);
        break;
    case SngTime:
        parseTime(buff, value);
        break;
    case SngDate:
        parseDate(buff, value);
        break;
    case SngTemp:
        parseTemp(buff, value);
        break;
    case SngValue:
        parseValue(buff, value);
        break;
    default:
        break;
    }
}


void SngMsgCreator::parseOnOff(char *buff, QString& value)
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

void SngMsgCreator::parseDimm(char *buff, QString& value)
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

void SngMsgCreator::parseTime(char *buff, QString& value)
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

void SngMsgCreator::parseDate(char *buff, QString& value)
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

void SngMsgCreator::parseTemp(char *buff, QString& value)
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

void SngMsgCreator::parseValue(char *buff, QString& value)
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
