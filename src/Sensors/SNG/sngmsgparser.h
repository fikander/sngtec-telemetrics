#ifndef SNGMSGPARSER_H
#define SNGMSGPARSER_H

#include "sngframe.h"

class SngMsgParser
{
public:
    SngMsgParser();
    bool parseMsg(char*, SngFrame&); // return true if msg invalid

private:
    bool checkBeginAndEndOfFrame(char*);
    bool getFrameType(char*, SngFrameType&);
    bool getValue(SngFrameType, char*, QString&);

    void getSrcAddr(char*, SngPhysicalAddress&);
    void getDestAddr(char*, SngPhysicalAddress&);

    bool parseOnOff(char*, QString&);   // correct values: on, off
    bool parseDimm(char*, QString&);    // inc, endInc, dec, endDec
    bool parseTime(char*, QString&);    // hh:mm:ss
    bool parseDate(char*, QString&);    // dd.mm.yyyy
    bool parseTemp(char*, QString&);    // [-]A.B , where A, B are from [0-255], minus optionaly
    bool parseValue(char*, QString&);   // 0-255
    bool checkZeros(char*, int);
    QString byteToString(char);
};

#endif // SNGMSGPARSER_H
