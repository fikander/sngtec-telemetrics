#ifndef SNGFRAME_H
#define SNGFRAME_H
#include "Sensors/SNG/sngphysicaladdress.h"

#define SNG_FRAME_TYPES_NR 6

enum SngFrameType
{
    SngOnOff, SngDimm, SngTime, SngDate, SngTemp, SngValue
};

static QString sngFrameTypeNames[] = {"OnOff", "Dimm", "Time", "Date", "Temp", "Value"};

class SngFrame
{
public:
    SngFrame();
    SngFrame(SngPhysicalAddress, SngPhysicalAddress, SngFrameType, QString);
    SngPhysicalAddress src;
    SngPhysicalAddress dest;
    SngFrameType type;
    QString value;

    QString toString();
};

#endif // SNGFRAME_H
