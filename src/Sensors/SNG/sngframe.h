#ifndef SNGFRAME_H
#define SNGFRAME_H
#include "src/Sensors/SNG/sngphysicaladdress.h"

#define SNG_FRAME_TYPES_NR 6

enum SngFrameType
{
    SngOnOff = 0, SngDimm = 1, SngTime = 2, SngDate = 3, SngTemp = 4, SngValue = 5
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
