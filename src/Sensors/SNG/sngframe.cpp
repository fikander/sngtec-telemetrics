#include "sngframe.h"

SngFrame::SngFrame()
{
}

SngFrame::SngFrame(SngPhysicalAddress src, SngPhysicalAddress dest, SngFrameType type, QString value) :
    src(src), dest(dest), type(type), value(value)
{
}

QString SngFrame::toString()
{
    QString res;
    res.append("src: ").append(src.toString());
    res.append(" dest: ").append(dest.toString());
    res.append(" type: ").append(sngFrameTypeNames[type]);
    res.append(" value: ").append(value);
    return res;
}
