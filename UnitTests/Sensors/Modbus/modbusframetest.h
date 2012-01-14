#ifndef MODBUSFRAMETEST_H
#define MODBUSFRAMETEST_H

#include "AutoTest.h"

class ModbusFrameTest : public QObject
{
    Q_OBJECT

private slots:
    void testCase1();
    void testCase1_data();
};

#ifdef MODBUS
DECLARE_TEST(ModbusFrameTest)
#endif

#endif // MODBUSFRAMETEST_H
