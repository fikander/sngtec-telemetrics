#ifndef SNGMSGCREATORTEST_H
#define SNGMSGCREATORTEST_H

#include "AutoTest.h"
#include <QString>
#include "src/Sensors/SNG/sngmsgcreator.h"

class SngMsgCreatorTest : public QObject
{
    Q_OBJECT

public:
    SngMsgCreatorTest() :
        defaultAddr(SngPhysicalAddress("0.0.0")), defaultType(SngValue), defaultValue("42") {};

private slots:
    void prepareMsgSrc();
    void prepareMsgSrc_data();
    void prepareMsgDest();
    void prepareMsgDest_data();
    void prepareMsgBeginEnd();

    void prepareMsgOnOff();
    void prepareMsgDimm();
    void prepareMsgDimm_data();
    void prepareMsgTime();
    void prepareMsgTime_data();
    void prepareMsgDate();
    void prepareMsgDate_data();
    void prepareMsgTemp();
    void prepareMsgTemp_data();
    void prepareMsgValue();
    void prepareMsgValue_data();


private:
    SngPhysicalAddress defaultAddr;
    SngFrameType defaultType;
    QString defaultValue;
};

#ifdef SNG
DECLARE_TEST(SngMsgCreatorTest);
#endif

#endif // SNGMSGCREATORTEST_H
