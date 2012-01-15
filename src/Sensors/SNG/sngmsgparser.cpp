#include "sngmsgparser.h"

SngMsgParser::SngMsgParser()
{
}

bool SngMsgParser::parseMsg(char * msg, SngFrame* frame)
{
    SngFrameType type;
    QString value;
    SngPhysicalAddress src, dest;

    if (checkBeginAndEndOfFrame(msg))
        return true;

    if (getFrameType(msg, type))
        return true;

    if (getValue(type, msg, value))
        return true;

    getSrcAddr(msg, src);
    getDestAddr(msg, dest);

    frame = new SngFrame(src, dest, type, value);

    return false;
}

bool SngMsgParser::checkBeginAndEndOfFrame(char * msg)
{
    return (msg[0] == '\xff') && (msg[1] == '\xff') && (msg[13] == '\xfe');
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

    int val = msg[0];
    val = (val+256)%256;

    res.sprintf("%d", val);
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
    //TODO
    return true;
}

bool SngMsgParser::parseDate(char * msg, QString & res)
{
    // TODO
    return true;
}

bool SngMsgParser::parseTemp(char * msg, QString & res)
{
    //TODO
    return true;
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
}

void SngMsgParser::getDestAddr(char * msg, SngPhysicalAddress & res)
{
}


