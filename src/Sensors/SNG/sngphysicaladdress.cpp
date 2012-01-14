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

char SngPhysicalAddress::getByteAt(int i)
{
    return bytes[i];
}

QVector<QString> SngPhysicalAddress::parseAddr(QString addr)
{
    QVector<QString> res;
    QString str = "";

    for (int i = 0; i < addr.size(); i++)
    {
        if (addr[i] == '.')
        {
            res.push_back(str);
            str.clear();
        }
        else
        {
            str.append(addr[i]);
        }
    }

    if (!(str.isEmpty()))
    {
        res.push_back(str);
    }

    return res;
}
