#ifndef SNGFRAMECREATOR_H
#define SNGFRAMECREATOR_H

#include "sngframe.h"

class SngMsgCreator
{
public:
    SngMsgCreator();
    void prepareMsg(SngFrame&, char*);

private:
    void setBeginAndEndOfFrame(char*);
    void setFrameType(SngFrameType, char*);
    void setSrcAddr(SngPhysicalAddress&, char*);
    void setDestAddr(SngPhysicalAddress&, char*);
    void setAddr(SngPhysicalAddress&, char*);

    //set value and methods used by one
    void setValue(SngFrameType, char*, QString&);
    void parseOnOff(char*, QString&);   // correct values: on, off
    void parseDimm(char*, QString&);    // inc, endInc, dec, endDec
    void parseTime(char*, QString&);    // hh:mm:ss
    void parseDate(char*, QString&);    // dd.mm.yyyy
    void parseTemp(char*, QString&);    // [-]A.B , where A, B are from [0-255], minus optionaly
    void parseValue(char*, QString&);   // 0-255
};

#endif // SNGFRAMECREATOR_H
