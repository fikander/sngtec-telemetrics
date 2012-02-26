#ifndef SNGPHYSICALADDRESS_H
#define SNGPHYSICALADDRESS_H

#include <QString>
#include <QVector>

class SngPhysicalAddress
{
private:
    char bytes[3];
    QString byteToString(char);

public:
    SngPhysicalAddress();
    SngPhysicalAddress(QString);
    char getByteAt(int);
    void setByteAt(int, char);

    QString toString();
};

#endif // SNGPHYSICALADDRESS_H
