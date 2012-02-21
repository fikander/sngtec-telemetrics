#ifndef SNGMSGPARSERTEST_H
#define SNGMSGPARSERTEST_H

#include "AutoTest.h"
#include <QString>
#include "src/Sensors/SNG/sngmsgparser.h"

class SngMsgParserTest : public QObject
{
    Q_OBJECT

public:
    SngMsgParserTest() :
        defaultAddr(SngPhysicalAddress("0.0.0")), defaultType(SngValue), defaultValue("42") {};

private slots:
    void parseMsgSrc();
    void parseMsgSrc_data();
    void parseMsgDest();
    void parseMsgDest_data();
    void parseMsgBeginEnd();

    void parseMsgOnOff();
    void parseMsgDimm();
    void parseMsgDimm_data();
    void parseMsgTime();
    void prepareMsgTime_data();
    void prepareMsgDate();
    void prepareMsgDate_data();
    void prepareMsgTemp();
    void prepareMsgTemp_data();
    void prepareMsgValue();
    void prepareMsgValue_data();

    void init(char*);


private:
    SngPhysicalAddress defaultAddr;
    SngFrameType defaultType;
    QString defaultValue;
};

#ifdef SNG
DECLARE_TEST(SngMsgParserTest);
#endif

#endif // SNGMSGPARSERTEST_H
