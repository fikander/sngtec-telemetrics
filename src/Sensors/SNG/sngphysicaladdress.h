#ifndef SNGPHYSICALADDRESS_H
#define SNGPHYSICALADDRESS_H

#include <QString>
#include <QVector>

class SngPhysicalAddress
{
private:
    char bytes[3];

public:
    SngPhysicalAddress();
    SngPhysicalAddress(QString);
    char getByteAt(int);

//    maybe be usefull in the future
//    QString toString();
};

#endif // SNGPHYSICALADDRESS_H
