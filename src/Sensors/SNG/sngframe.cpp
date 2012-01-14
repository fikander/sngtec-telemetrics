#include "sngframe.h"

SngFrame::SngFrame(SngPhysicalAddress src, SngPhysicalAddress dest, SngFrameType type, QString value) :
    src(src), dest(dest), type(type), value(value)
{
}
