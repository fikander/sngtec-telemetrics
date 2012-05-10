#include "sngmsgparser.h"
#include <QDebug>

SngMsgParser::SngMsgParser()
{
}

bool SngMsgParser::parseMsg(char * msg, SngFrame& frame)
{
    SngFrameType type;
    QString value;

    if (checkBeginAndEndOfFrame(msg))
    {
        qDebug() << "SngMsgParser::parseMsg: wrong begin and/or end of frame";
        return true;
    }

    if (getFrameType(msg, type))
    {
        QString ft;
        qDebug() << "SngMsgParser::parseMsg: wrong frame type";
        return true;
    }

    if (getValue(type, msg, value))
    {
        qDebug() << "SngMsgParser::parseMsg: wrong value";
        return true;
    }

    getSrcAddr(msg, frame.src);
    getDestAddr(msg, frame.dest);

    frame.type = type;
    frame.value = value;

    return false;
}

bool SngMsgParser::checkBeginAndEndOfFrame(char * msg)
{
    return (msg[0] != '\xff') || (msg[1] != '\xff') || (msg[13] != '\xfe');
}

bool SngMsgParser::getFrameType(char * msg, SngFrameType & frameType)
{
    char type = msg[8];

    switch (type) {
    case '\x01':
        frameType = SngOnOff;
        return false;
    case '\x02':
        frameType = SngDimm;
        return false;
    case '\x03':
        frameType = SngTime;
        return false;
    case '\x04':
        frameType = SngDate;
        return false;
    case '\x05':
        frameType = SngTemp;
        return false;
    case '\x06':
        frameType = SngValue;
        return false;
    default:
        return true;
    }
}

bool SngMsgParser::getValue(SngFrameType frameType, char * msg, QString &res)
{
    char * buff = &msg[9];
    switch (frameType) {
    case SngOnOff:
        return parseOnOff(buff, res);
    case SngDimm:
        return parseDimm(buff, res);
    case SngTime:
        return parseTime(buff, res);
    case SngDate:
        return parseDate(buff, res);
    case SngTemp:
        return parseTemp(buff, res);
    case SngValue:
        return parseValue(buff, res);
    default:
        return true;
    }
}

bool SngMsgParser::parseOnOff(char * msg, QString & res)
{
    if (msg[0] == '\x00')
    {
        res = "off";
    }
    else if (msg[0] == '\x01')
    {
        res = "on";
    }
    else
    {
        return true;
    }

    return checkZeros(&msg[1], 3);
}

bool SngMsgParser::parseDimm(char * msg, QString & res)
{
    if (msg[0] == '\x00')
    {
        res = "endDec";
    }
    else if (msg[0] == '\x01')
    {
        res = "dec";
    }
    else if (msg[0] == '\x08')
    {
        res = "endInc";
    }
    else if (msg[0] == '\x09')
    {
        res = "inc";
    }
    else
    {
        return true;
    }

    return checkZeros(&msg[1], 3);
}

bool SngMsgParser::parseTime(char * msg, QString & res)
{
    QString hours = byteToString(msg[0]);
    QString min = byteToString(msg[1]);
    QString sec = byteToString(msg[2]);
    int h = hours.toInt();
    int m = min.toInt();
    int s = sec.toInt();

    if (h > 23)
        return true;
    else
        res.append(hours).append(":");

    if (m > 59)
        return true;
    else if (m < 10)
        res.append("0");

    res.append(min).append(":");

    if (s > 59)
        return true;
    else if(s < 10)
        res.append("0");

    res.append(sec);

    return checkZeros(&msg[3], 1);
}

bool SngMsgParser::parseDate(char * msg, QString & res)
{
    QString days = byteToString(msg[0]);
    QString months = byteToString(msg[1]);
    QString years = byteToString(msg[2]);
    int d = days.toInt();
    int m = months.toInt();

    if ((d > 31) || (d == 0))
        return true;
    else if (d < 10)
        res.append("0");
    else
        res.append(days).append(".");

    if ((m > 12) || (m == 0))
        return true;
    else if (m < 10)
        res.append("0");

    res.append(months).append(".");

    res.append(years);

    return checkZeros(&msg[3], 1);
}

bool SngMsgParser::parseTemp(char * msg, QString & res)
{
    res.clear();

    if (msg[2] == '\x01')
    {
        res.append("-");
    }
    else if (msg[2] != '\x00')
    {
        return true;
    }

    res.append(byteToString(msg[0]));
    res.append(".");
    res.append(byteToString(msg[1]));

    return checkZeros(&msg[3], 1);
}

bool SngMsgParser::parseValue(char * msg, QString & res)
{
    res.clear();
    res.append(byteToString(msg[0]));

    return checkZeros(&msg[1], 3);
}

bool SngMsgParser::checkZeros(char * buff, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (buff[i] != '\x00')
            return true;
    }
    return false;
}

QString SngMsgParser::byteToString(char c)
{
    QString res;
    int val = c;

    val = (val + 256) % 256;

    res.sprintf("%d", val);
    return res;
}


void SngMsgParser::getSrcAddr(char * msg, SngPhysicalAddress & res)
{
    getAddr(&msg[2], res);
}

void SngMsgParser::getDestAddr(char * msg, SngPhysicalAddress & res)
{
    getAddr(&msg[5], res);
}

void SngMsgParser::getAddr(char * msg, SngPhysicalAddress & res)
{
    for (int i = 0; i <= 2; i++)
    {
        res.setByteAt(i, msg[i]);
    }
}


