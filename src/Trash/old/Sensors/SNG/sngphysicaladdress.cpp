#include "sngphysicaladdress.h"
#include <QStringList>

SngPhysicalAddress::SngPhysicalAddress()
{
    bytes[0] = bytes[1] = bytes[2] = 0;
}

SngPhysicalAddress::SngPhysicalAddress(QString addr)
{
    QStringList parsedAdrr = addr.split(".");
    for (int i = 0; i < 3; i++)
    {
        bytes[i] = parsedAdrr[i].toInt();
    }
}

void SngPhysicalAddress::make(QString addr)
{
    QStringList parsedAdrr = addr.split(".");
    for (int i = 0; i < 3; i++)
    {
        bytes[i] = parsedAdrr[i].toInt();
    }
}

bool SngPhysicalAddress::isWrongAddr(QString & addr)
{
    QStringList parsedAdrr = addr.split(".");
    if (parsedAdrr.length() != 3)
    {
        return true;
    }
    bool ok = true;

    for (int i = 0; i < 3; i++)
    {
        parsedAdrr[i].toInt(&ok, 10);
        if (!ok)
        {
            return true;
        }
    }

    return false;
}

char SngPhysicalAddress::getByteAt(int i)
{
    return bytes[i];
}

void SngPhysicalAddress::setByteAt(int i, char c)
{
    bytes[i] = c;
}

QString SngPhysicalAddress::toString()
{
    QString res;
    res.append(byteToString(bytes[0]));
    res.append(".");
    res.append(byteToString(bytes[1]));
    res.append(".");
    res.append(byteToString(bytes[2]));
    return res;
}

QString SngPhysicalAddress::byteToString(char c)
{
    QString res;
    int val = c;

    val = (val + 256) % 256;

    res.sprintf("%d", val);
    return res;
}
